#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/oom.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

struct mem_usage_stats {
    int pid;               // PID del proceso
    unsigned long vmsize;  // Memoria reservada (VmSize)
    unsigned long vmrss;   // Memoria comprometida (VmRSS)
    int oom_score;         // Puntuación OOM
};

SYSCALL_DEFINE1(julioz_get_memory_usage_stats, int, pid)
{
    struct task_struct *task;
    struct mem_usage_stats stats;
    struct mem_usage_stats *user_buffer;
    int ret = 0;

    // Validar el PID recibido
    if (pid < 0) {
        return -EINVAL; // Error: PID inválido
    }

    // Si pid == 0, iterar sobre todos los procesos
    if (pid == 0) {
        for_each_process(task) {
            stats.pid = task->pid;

            if (task->mm) {
                stats.vmsize = task->mm->total_vm << PAGE_SHIFT;
                stats.vmrss = get_mm_rss(task->mm) << PAGE_SHIFT;
            } else {
                stats.vmsize = 0;
                stats.vmrss = 0;
            }

            stats.oom_score = oom_badness(task, NULL, 0);

            // Copiar los datos al espacio de usuario (pendiente: buffer dinámico)
            ret = copy_to_user(user_buffer, &stats, sizeof(stats));
            if (ret != 0) {
                return -EFAULT;
            }
        }
    } else {
        // Buscar el proceso correspondiente al PID
        task = find_task_by_vpid(pid);
        if (!task) {
            return -ESRCH; // Error: Proceso no encontrado
        }

        stats.pid = task->pid;

        if (task->mm) {
            stats.vmsize = task->mm->total_vm << PAGE_SHIFT;
            stats.vmrss = get_mm_rss(task->mm) << PAGE_SHIFT;
        } else {
            stats.vmsize = 0;
            stats.vmrss = 0;
        }

        stats.oom_score = oom_badness(task, NULL, 0);

        // Copiar los datos al espacio de usuario
        ret = copy_to_user(user_buffer, &stats, sizeof(stats));
        if (ret != 0) {
            return -EFAULT;
        }
    }

    return 0;
}
