#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/oom.h>
#include <linux/uaccess.h> // Para copy_to_user

struct mem_usage_stats {
    int pid;                // PID del proceso
    unsigned long vmsize;   // Memoria reservada (VmSize)
    unsigned long vmrss;    // Memoria comprometida (VmRSS)
    int oom_score;          // Puntuación OOM
    int committed_percent;  // Porcentaje de memoria utilizada
};

SYSCALL_DEFINE3(julioz_get_memory_usage_stats, int, pid, struct mem_usage_stats __user *, buffer, size_t, max_entries) {
    struct task_struct *task;
    struct mem_usage_stats stats;
    size_t count = 0;

    if (pid < 0 || !buffer || max_entries <= 0) {
        return -EINVAL; // Validación de entrada
    }

    if (pid == 0) {
        // Iterar sobre todos los procesos
        for_each_process(task) {
            if (count >= max_entries) {
                break; // Evitar desbordamiento del buffer
            }

            stats.pid = task->pid;
            if (task->mm) {
                stats.vmsize = (task->mm->total_vm << PAGE_SHIFT) / 1024;
                stats.vmrss = (get_mm_rss(task->mm) << PAGE_SHIFT) / 1024;

                // Calcular porcentaje de memoria utilizada
                stats.committed_percent = (stats.vmsize > 0) 
                                           ? (stats.vmrss * 100) / stats.vmsize 
                                           : 0;
            } else {
                stats.vmsize = 0;
                stats.vmrss = 0;
                stats.committed_percent = 0;
            }
            
            // Usar totalram_pages como base para oom_score
            stats.oom_score = oom_badness(task, totalram_pages);

            // Copiar la estructura al espacio de usuario
            if (copy_to_user(&buffer[count], &stats, sizeof(struct mem_usage_stats))) {
                return -EFAULT; // Error al copiar
            }
            count++;
        }
    } else {
        // Obtener información de un proceso específico
        task = find_task_by_vpid(pid);
        if (!task) {
            return -ESRCH; // Proceso no encontrado
        }

        stats.pid = task->pid;
        if (task->mm) {
            stats.vmsize = (task->mm->total_vm << PAGE_SHIFT) / 1024;
            stats.vmrss = (get_mm_rss(task->mm) << PAGE_SHIFT) / 1024;

            // Calcular porcentaje de memoria utilizada
            stats.committed_percent = (stats.vmsize > 0) 
                                       ? (stats.vmrss * 100) / stats.vmsize 
                                       : 0;
        } else {
            stats.vmsize = 0;
            stats.vmrss = 0;
            stats.committed_percent = 0;
        }

        // Usar totalram_pages como base para oom_score
        stats.oom_score = oom_badness(task, totalram_pages);

        // Copiar la estructura al espacio de usuario
        if (copy_to_user(buffer, &stats, sizeof(struct mem_usage_stats))) {
            return -EFAULT; // Error al copiar
        }
        count = 1;
    }

    return count; // Número de entradas copiadas
}
