#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/uaccess.h> // Para copy_to_user

struct global_mem_stats {
    unsigned long total_reserved_mb;  // Memoria total reservada (VmSize) en MB
    unsigned long total_committed_mb; // Memoria total utilizada (VmRSS) en MB
};

SYSCALL_DEFINE1(julioz_get_global_memory_usage_stats, struct global_mem_stats __user *, user_stats) {
    struct task_struct *task;
    struct global_mem_stats stats = {0, 0}; // Inicializar a 0

    // Iterar sobre todos los procesos
    for_each_process(task) {
        if (task->mm) {
            stats.total_reserved_mb += (task->mm->total_vm << PAGE_SHIFT) / (1024 * 1024); // Convertir a MB
            stats.total_committed_mb += (get_mm_rss(task->mm) << PAGE_SHIFT) / (1024 * 1024); // Convertir a MB
        }
    }

    // Copiar los resultados al espacio de usuario
    if (copy_to_user(user_stats, &stats, sizeof(struct global_mem_stats))) {
        return -EFAULT; // Error al copiar
    }

    return 0; // Éxito
}


