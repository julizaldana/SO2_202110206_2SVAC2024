#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/oom.h>
#include <linux/uaccess.h> // Para copy_to_user
#include <linux/swap.h>

struct mem_usage_stats {
    int pid;                // PID del proceso
    unsigned long vmsize;   // Memoria reservada (VmSize)
    unsigned long vmrss;    // Memoria comprometida (VmRSS)
    int oom_score;          // Puntuación OOM
    int committed_percent;  // Porcentaje de memoria utilizada
};

int calculate_oom_score(struct task_struct *task) {
    unsigned long rss = 0;
    unsigned long totalpages = totalram_pages + total_swap_pages;
    long adj = 0;

    // Obtener memoria residente (VmRSS)
    if (task->mm) {
        rss = get_mm_rss(task->mm);
    }

    // Ajustar por oom_score_adj
    if (task->signal) {
        adj = task->signal->oom_score_adj;
    }

    // Calcular el puntaje OOM
    int score = (rss * 1000) / totalpages;
    score += adj;

    // Asegurar límites válidos
    if (score < 0) {
        score = 0;
    } else if (score > 1000) {
        score = 1000;
    }

    return score;
}

SYSCALL_DEFINE3(julioz_get_memory_usage_stats, int, pid, struct mem_usage_stats __user *, buffer, size_t, max_entries) {
    struct task_struct *task;
    struct mem_usage_stats stats;
    size_t count = 0;

    if (pid < 0 || !buffer || max_entries <= 0) {
        return -EINVAL; // Validación de entrada
    }

    if (pid == 0) {
        // Itera sobre todos los procesos
        for_each_process(task) {
            if (count >= max_entries) {
                break; // Evita el desbordamiento del buffer
            }

            stats.pid = task->pid;
            if (task->mm) {
                stats.vmsize = (task->mm->total_vm << PAGE_SHIFT) / 1024;
                stats.vmrss = (get_mm_rss(task->mm) << PAGE_SHIFT) / 1024;

                // Calcula porcentaje de memoria utilizada
                stats.committed_percent = (stats.vmsize > 0) 
                                           ? (stats.vmrss * 100) / stats.vmsize 
                                           : 0;
            } else {
                stats.vmsize = 0;
                stats.vmrss = 0;
                stats.committed_percent = 0;
            }
            
            // Calcular oom_score
            stats.oom_score = calculate_oom_score(task);

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

        // Calcular oom_score
        stats.oom_score = calculate_oom_score(task);

        // Copiar la estructura al espacio de usuario
        if (copy_to_user(buffer, &stats, sizeof(struct mem_usage_stats))) {
            return -EFAULT; // Error al copiar
        }
        count = 1;
    }

    return count; // Número de entradas copiadas
}
