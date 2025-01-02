#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#include "crudmemlimit.h"

//AGREGAR ENTRADAS DE PROCESOS NUEVOS A SER LIMITADOS
//SUDOERS
//CADA ENTRADA A LA LISTA DEBE HACER USO DEL STRUCT:

struct memory_limitation {
    pid_t pid;
    size_t memory_limit;
};

struct memory_limit_entry 
{
    struct memory_limitation data;
    struct list_head list;
};

static LIST_HEAD(memory_list);

static DEFINE_MUTEX(memory_list_mutex);


bool is_request_accepted(pid_t pid, size_t requested_memory) {
    //Revisar si le acepta/denega la solicitud al proceso
    struct memory_limit_entry *entry;
    struct task_struct *task;
    size_t current_usage;

    task = find_task_by_vpid(pid);
    if (!task) {
        printk(KERN_ERR "INVALID PID SEARCH? PROCESS DIED AFTER REQUESTING MEMORY????");
        return false;
    }

    if (!task->mm) {
        printk(KERN_ERR "INVALID PID total_vm VALUE???? SHOULD NOT HAPPEN?");
        return false;
    }

    current_usage = (task->mm->total_vm << PAGE_SHIFT);

    mutex_lock(&memory_list_mutex);
    list_for_each_entry(entry, &memory_list, list) {
        if (entry->data.pid == pid) {
            if (current_usage + requested_memory > entry->data.memory_limit) {
                mutex_unlock(&memory_list_mutex);
                printk(KERN_INFO "Llamada denegada para el PID %d\n", pid);
                return false;
            }
            mutex_unlock(&memory_list_mutex);
            printk(KERN_INFO "USAC-Project3-> Allowed call for PID %d, total_vm is now %zu\n", pid, current_usage);
            return true;
            break;
        }
    }
    mutex_unlock(&memory_list_mutex);
    return true;
}


//CREATE

SYSCALL_DEFINE2(julioz_add_memory_limit, pid_t, process_pid, size_t, memory_limit)
{
    struct memory_limit_entry *entry;
    struct memory_limit_entry *existing_entry;
    struct task_struct *task;

    // Se valida que el pid y la cantidad de memoria limitada no sean negativos
    if (process_pid < 0 || memory_limit < 0) {
        return -EINVAL; // Argumento inválido
    }

    // Se verifica si el usuario es sudoer
    if (!capable(CAP_SYS_ADMIN)) {
        return -EPERM; // Permiso denegado
    }

    // Se verifica si el pid del proceso existe
    task = find_task_by_vpid(process_pid);
    if (!task) {
        return -ESRCH; // Proceso no encontrado
    }

    // Se verifica si ya existe el proceso en la lista
    mutex_lock(&memory_list_mutex);
    list_for_each_entry(existing_entry, &memory_list, list) {
        if (existing_entry->data.pid == process_pid) {
            mutex_unlock(&memory_list_mutex);
            return -101; // Proceso ya en la lista
        }
    }

    // Se intenta asignar memoria para el nuevo nodo
    entry = kmalloc(sizeof(struct memory_limit_entry), GFP_KERNEL);
    if (!entry) {
        mutex_unlock(&memory_list_mutex);
        return -ENOMEM; // Memoria insuficiente
    }

    // Inicializar y agregar el nodo a la lista
    entry->data.pid = process_pid;
    entry->data.memory_limit = memory_limit;
    list_add(&entry->list, &memory_list);
    mutex_unlock(&memory_list_mutex);

    return 0; 
}



//OBTENER LA LISTA DE PROCESOS QUE HAN SIDO LIMITADOS
//PASAR EL PUNTERO A UN STRUCT DONDE EL KERNEL ESCRIBIRÁ LOS PROCESOS

//READ


SYSCALL_DEFINE3(julioz_get_memory_limits, struct memory_limitation*, u_processes_buffer, size_t, max_entries, int*, processes_returned)
{

    //Todo: manejo de errores, argumentos, sudoers, etc.
    //para todos los usuarios

    //pide memoria
    struct memory_limit_entry * entry;
    int count = 0;


    //Memoria para el buffer del lado del kernel
    //alojar memoria
    struct memory_limitation* k_processes_buffer = kmalloc_array(max_entries, sizeof(struct memory_limitation), GFP_KERNEL); 
    if (!k_processes_buffer) {
        return -ENOMEM;
    }


    //iterar datos
    mutex_lock(&memory_list_mutex);
    list_for_each_entry(entry, &memory_list, list) {
        if (count >= max_entries) {
            break;
        }

        k_processes_buffer[count].pid = entry->data.pid;
        k_processes_buffer[count].memory_limit = entry->data.memory_limit;
        count++;
    }
    mutex_unlock(&memory_list_mutex);

    //devolver al usuario
    if (copy_to_user(u_processes_buffer, k_processes_buffer, count*sizeof(struct memory_limitation))) {
        kfree(k_processes_buffer);
        return -EFAULT;
    }

    if (put_user(count, processes_returned)) {
        kfree(k_processes_buffer);
        return -EFAULT;
    }

    kfree(k_processes_buffer);


    return 0;
}

//UPDATE

SYSCALL_DEFINE2(julioz_update_memory_limit, pid_t, process_pid, size_t, memory_limit)
{
    struct memory_limit_entry *entry;

    // Se valida que el pid y la cantidad de memoria limitada no sean negativos
    if (process_pid < 0 || memory_limit < 0) {
        return -EINVAL; // Argumento inválido
    }


    // Verificar si el usuario es sudoer
    if (!capable(CAP_SYS_ADMIN)) {
        return -EPERM; // Permiso denegado
    }

    // Buscar el proceso en la lista y actualizarlo
    mutex_lock(&memory_list_mutex);
    list_for_each_entry(entry, &memory_list, list) {
        if (entry->data.pid == process_pid) {
            entry->data.memory_limit = memory_limit;
            mutex_unlock(&memory_list_mutex);
            return 0; // Update exitoso
        }
    }
    mutex_unlock(&memory_list_mutex);

    // Si no se encontró el proceso
    return -ESRCH; // Proceso no encontrado
}



//DELETE


SYSCALL_DEFINE1(julioz_remove_memory_limit, pid_t, process_pid)
{
    struct memory_limit_entry *entry, *tmp;

    // Se valida que el pid no sea negativo
    if (process_pid < 0) {
        return -EINVAL; // PID negativo
    }

    // Verificar si el usuario es sudoer
    if (!capable(CAP_SYS_ADMIN)) {
        return -EPERM; // Permiso denegado
    }

    // Buscar el proceso en la lista y eliminarlo
    mutex_lock(&memory_list_mutex);
    list_for_each_entry_safe(entry, tmp, &memory_list, list) {
        if (entry->data.pid == process_pid) {
            list_del(&entry->list);
            kfree(entry);
            mutex_unlock(&memory_list_mutex);
            return 0; // Delete exitoso
        }
    }
    mutex_unlock(&memory_list_mutex);

    // Si no se encontró el proceso
    return -ESRCH; // Proceso no encontrado
}





//Manejo de errores.