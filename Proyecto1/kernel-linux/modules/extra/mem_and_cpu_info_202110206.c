#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h>
#include <linux/kernel_stat.h>
#include <linux/tick.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Julio Zaldaña");
MODULE_DESCRIPTION("Módulo para leer información de Memoria y CPU");
MODULE_VERSION("1.1");

#define PROC_NAME "mem_and_cpu_info_202110206"

// Estructura para almacenar jiffies iniciales
static u64 prev_idle = 0, prev_total = 0;

// Función para calcular el uso de CPU
static unsigned long calculate_cpu_usage(void) {
    u64 user, nice, system, idle, iowait, irq, softirq, steal;
    u64 total, diff_idle, diff_total, usage;

    // Obtener tiempos de CPU (en jiffies)
    user = kcpustat_cpu(0).cpustat[CPUTIME_USER];
    nice = kcpustat_cpu(0).cpustat[CPUTIME_NICE];
    system = kcpustat_cpu(0).cpustat[CPUTIME_SYSTEM];
    idle = kcpustat_cpu(0).cpustat[CPUTIME_IDLE];
    iowait = kcpustat_cpu(0).cpustat[CPUTIME_IOWAIT];
    irq = kcpustat_cpu(0).cpustat[CPUTIME_IRQ];
    softirq = kcpustat_cpu(0).cpustat[CPUTIME_SOFTIRQ];
    steal = kcpustat_cpu(0).cpustat[CPUTIME_STEAL];

    // Calcular el total de jiffies
    total = user + nice + system + idle + iowait + irq + softirq + steal;

    // Diferencias respecto al anterior cálculo
    diff_idle = idle - prev_idle;
    diff_total = total - prev_total;

    // Actualizar valores previos
    prev_idle = idle;
    prev_total = total;

    // Calcular porcentaje de uso
    usage = 0;
    if (diff_total > 0)
        usage = (100 * (diff_total - diff_idle)) / diff_total;

    return usage;
}

// Mostrar la información en /proc
static int memandprocessesinfo_show(struct seq_file *m, void *v) {
    struct sysinfo si;

    // Información de memoria
    si_meminfo(&si);
    seq_printf(m, "\n--- Información de Memoria ---\n");
    seq_printf(m, "Total RAM: %lu KB\n", si.totalram * 4);
    seq_printf(m, "Free RAM: %lu KB\n", si.freeram * 4);

    // Uso de CPU
    unsigned long cpu_usage = calculate_cpu_usage();
    seq_printf(m, "\n--- Información de CPU ---\n");
    seq_printf(m, "Uso de CPU: %lu%%\n", cpu_usage);

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
    // Inicializar valores de jiffies previos
    prev_idle = kcpustat_cpu(0).cpustat[CPUTIME_IDLE];
    prev_total = kcpustat_cpu(0).cpustat[CPUTIME_USER] +
                 kcpustat_cpu(0).cpustat[CPUTIME_NICE] +
                 kcpustat_cpu(0).cpustat[CPUTIME_SYSTEM] +
                 kcpustat_cpu(0).cpustat[CPUTIME_IDLE] +
                 kcpustat_cpu(0).cpustat[CPUTIME_IOWAIT] +
                 kcpustat_cpu(0).cpustat[CPUTIME_IRQ] +
                 kcpustat_cpu(0).cpustat[CPUTIME_SOFTIRQ] +
                 kcpustat_cpu(0).cpustat[CPUTIME_STEAL];

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
