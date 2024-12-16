#include <linux/kernel>
#include <linux/syscalls.h>

// GET MEMORY SNAPSHOT (IMPLEMENTADO EN /KERNEL/SYS.C)

struct mem_snapshot {
    u64 total_memory;   // Total de memoria física
    u64 free_memory;    // Memoria libre
    u64 total_swap;     // Total de swap
    u64 free_swap;      // Swap libre
    u64 active_pages;   // Páginas activas
    u64 cached_pages;   // Páginas en caché
};

SYSCALL_DEFINE1(julioz_capture_memory_snapshot, struct mem_snapshot __user *, user_snapshot)
{
    struct mem_snapshot snapshot;
    struct sysinfo sysinfo;
    unsigned long cached, active;

    // Obtenemos información general del sistema
    si_meminfo(&sysinfo);

    snapshot.total_memory = sysinfo.totalram << (PAGE_SHIFT - 10); // KB
    snapshot.free_memory = sysinfo.freeram << (PAGE_SHIFT - 10);   // KB
    snapshot.total_swap = sysinfo.totalswap << (PAGE_SHIFT - 10); // KB
    snapshot.free_swap = sysinfo.freeswap << (PAGE_SHIFT - 10);   // KB

    // Obtenemos estadísticas de páginas activas y en caché
    cached = global_node_page_state(NR_FILE_PAGES) - total_swapcache_pages();
    cached = (cached < 0) ? 0 : cached;
    snapshot.cached_pages = cached << (PAGE_SHIFT - 10); // KB

    active = global_node_page_state(NR_ACTIVE_ANON) +
             global_node_page_state(NR_ACTIVE_FILE);
    snapshot.active_pages = active << (PAGE_SHIFT - 10); // KB

    // Copiamos el resultado al espacio de usuario
    if (copy_to_user(user_snapshot, &snapshot, sizeof(struct mem_snapshot)))
        return -EFAULT;

    return 0; // Éxito
}