#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>  // Para crear archivos en /proc
#include <linux/seq_file.h> // Para escribir en archivos
#include <linux/mm.h>       // Para manejar información de memoria
#include <linux/sched.h>    // Para manejar procesos
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/fs.h>       // Para estadísticas de almacenamiento
#include <linux/statfs.h>   // Para estructuras de estadísticas de sistema de archivos

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Julio Zaldaña");
MODULE_DESCRIPTION("Módulo para leer información de Memoria, uso de CPU y almacenamiento");
MODULE_VERSION("1.0");

#define PROC_NAME "mem_and_cpu_info_202110206"

// Variables para calcular estadísticas de CPU
static unsigned long prev_idle = 0;
static unsigned long prev_total = 0;

/* Función para calcular el uso de CPU */
static unsigned long calculate_cpu_usage(void) {
    unsigned long cpu_user, cpu_nice, cpu_system, cpu_idle;
    unsigned long total, idle, usage;

    // Leer valores de /proc/stat
    cpu_user = cpu_nice = cpu_system = cpu_idle = 0;
    total = idle = usage = 0;

    struct file *f;
    mm_segment_t fs;
    char buf[128];
    int read;

    f = filp_open("/proc/stat", O_RDONLY, 0);
    if (IS_ERR(f)) {
        printk(KERN_ERR "Error al abrir /proc/stat\n");
        return 0;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);

    read = kernel_read(f, buf, sizeof(buf) - 1, &f->f_pos);
    set_fs(fs);
    filp_close(f, NULL);

    if (read <= 0) {
        printk(KERN_ERR "Error al leer /proc/stat\n");
        return 0;
    }

    buf[read] = '\0';

    sscanf(buf, "cpu %lu %lu %lu %lu", &cpu_user, &cpu_nice, &cpu_system, &cpu_idle);

    total = cpu_user + cpu_nice + cpu_system + cpu_idle;
    idle = cpu_idle;

    // Calcular el porcentaje de uso de CPU
    usage = 0;
    if (total - prev_total > 0)
        usage = 100 * (1 - (idle - prev_idle) / (float)(total - prev_total));

    prev_idle = idle;
    prev_total = total;

    return usage;
}

/* Función para obtener estadísticas de almacenamiento */
static void get_disk_info(struct seq_file *m, const char *path) {
    struct kstatfs stats;

    if (vfs_statfs(path, &stats) == 0) {
        unsigned long total_space = (stats.f_blocks * stats.f_bsize) >> 10; // KB
        unsigned long free_space = (stats.f_bfree * stats.f_bsize) >> 10;  // KB

        seq_printf(m, "\n--- Información de Almacenamiento (%s) ---\n", path);
        seq_printf(m, "Espacio Total: %lu KB\n", total_space);
        seq_printf(m, "Espacio Libre: %lu KB\n", free_space);
    } else {
        seq_printf(m, "\nNo se pudo obtener información del disco para %s\n", path);
    }
}

static int memandprocessesinfo_show(struct seq_file *m, void *v) {
    struct sysinfo si;

    si_meminfo(&si);

    // Información de memoria
    seq_printf(m, "\n--- Información de Memoria ---\n");
    seq_printf(m, "Total RAM: %lu KB\n", si.totalram * 4);
    seq_printf(m, "Free RAM: %lu KB\n", si.freeram * 4);

    // Uso de CPU
    seq_printf(m, "\n--- Estadísticas de CPU ---\n");
    seq_printf(m, "Uso de CPU: %lu%%\n", calculate_cpu_usage());

    // Información de almacenamiento
    get_disk_info(m, "/");

    return 0;
}

static int memandprocessesinfo_open(struct inode *inode, struct file *file) {
    return single_open(file, memandprocessesinfo_show, NULL);
}

static const struct proc_ops memandprocessesinfo_ops = {
    .proc_open = memandprocessesinfo_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init memandprocessesinfo_init(void) {
    proc_create(PROC_NAME, 0, NULL, &memandprocessesinfo_ops);
    printk(KERN_INFO "Módulo cargado: %s\n", PROC_NAME);
    return 0;
}

static void __exit memandprocessesinfo_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "Módulo descargado: %s\n", PROC_NAME);
}

module_init(memandprocessesinfo_init);
module_exit(memandprocessesinfo_exit);
