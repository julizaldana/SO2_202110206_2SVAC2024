#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

struct io_stats {
    u64 bytes_read;
    u64 bytes_written;
    u64 cancel_count;     
    u64 io_wait_time;
    u64 disk_io_time;
};

SYSCALL_DEFINE1(julioz_get_io_throttle, struct io_stats __user *, user_stats) {
    struct io_stats stats = {0};
    struct task_struct *task = current;

    // Obtenemos estadísticas de I/O si están disponibles
#if IS_ENABLED(CONFIG_TASK_IO_ACCOUNTING)
    stats.bytes_read = task->ioac.read_bytes;
    stats.bytes_written = task->ioac.write_bytes;
    stats.cancel_count = task->ioac.cancelled_write_bytes; 
#else
    stats.bytes_read = 0;
    stats.bytes_written = 0;
    stats.cancel_count = 0;
#endif

    // Placeholder para estadísticas adicionales
    stats.io_wait_time = 0;  // Implementa si puedes medirlo
    stats.disk_io_time = 0;  // Implementa si puedes medirlo

    // Copiamos la estructura al espacio de usuario
    if (copy_to_user(user_stats, &stats, sizeof(struct io_stats))) {
        return -EFAULT;
    }

    return 0;
}
