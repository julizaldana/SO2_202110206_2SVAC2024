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


bool is_request_accepted(pid_t pid; size_t requested_memory) {
    //Revisar si le acepta/denega la solicitud al proceso
    struct memory_limit_entry *entry;
    struct task_struct *task;
    size_t current_usage;

    task = find_task_by_vpid(pid);
    if (!task) {
        printk(KERN_ERR "INVALID PID SEARCH? PROCESS DIED AFTER REQUESTING MEMORY????")
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
            printk(KERN_INFO "USAC-Project3-> Allowed call for PID %d, total_vm is now %zu\n", pid, current_usage)
            return true;
            break;
        }
    }
    mutex_unlock(&memory_list_mutex);
    return true;
}


SYSCALL_DEFINE2(julioz_add_memory_limit, pid_t, process_pid, size_t, memory_limit)
{

    //pide memoria
    struct memory_limit_entry * entry;
    entry = kmalloc(sizeof(struct memory_limit_entry), GFP_KERNEL);
    if (!entry) {
        return -ENOMEM;
    }

    //llena el nuevo elemento
    entry->data.pid = process_pid;
    entry->data.memory_limit = memory_limit;

    //agregar a la lista
    mutex_lock(&memory_list_mutex);
    list_add(&entry->list,&memory_list);
    mutex_unlock(&memory_list_mutex);

    return 0;

}


//OBTENER LA LISTA DE PROCESOS QUE HAN SIDO LIMITADOS
//PASAR EL PUNTERO A UN STRUCT DONDE EL KERNEL ESCRIBIRÁ LOS PROCESOS


SYSCALL_DEFINE3(julioz_get_memory_limits, struct memory_limitation*, u_processes_buffer, size_t, max_entries, int*, processes_returned)
{

    //Todo: manejo de errores, argumentos, sudoers, etc.
    //para todos los usuarios

    //pide memoria
    struct memory_limit_entry * entry;
    int count = 0;

    //Memoria para el buffer del lado del kernel
    //alojar memoria
    struct memory_limitation* k_processes_buffer = kmalloc_array(max_entries, sizeof(struct memory_limitation), GFP_KERNEL) 
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
        return -EFAULT
    }

    if (put_user(count, processes_returned)) {
        kfree(k_processes_buffer);
        return -EFAULT
    }

    kfree(k_processes_buffer);


    return 0;
}


SYSCALL_DEFINE2(julioz_update_memory_limit, pid_t, process_pid, size_t, memory_limit)
{
    //TODO manejo de argumentos, errores
    //Sudoers

    //pide memoria
    struct memory_limit_entry * entry;

    //verificar pid si existe, igualarlo, actualizar o sobreescribir datos
    mutex_lock(&memory_limit_mutex);
    list_for_each_entry(entry, &memory_list, list) {
        if (entry->data.pid == process_pid) {
            entry->data.memory_limit = memory_limit;
            mutex_unlock(&memory_list_mutex);

            return 0;
        }
    }
    mutex_unlock(&memory_limit_mutex);
    return -ESRCH;

}





SYSCALL_DEFINE1(julioz_remove_memory_limit, pid_t, process_pid)
{

    //Todo: manejo de errores y argumentos
    //Todo: Sudoer

    //pide memoria
    struct memory_limit_entry * entry, * tmp;

    mutex_lock(&memory_limit_mutex);

    //buscar pid si existe, si existe, liberarlo y eliminarlo de la lista
    list_for_each_entry_safe(entry, tmp, &memory_list, list) {
        if (entry->data.pid == process_pid) {
            list_del(&entry->list);
            kfree(entry);
            mutex_unlock(&memory_list_mutex);

            //print_memory_limitation_list();
            return 0;
        }
    }

    mutex_unlock(&memory_list_mutex);
    return -ESRCH;

}




//Manejo de errores.