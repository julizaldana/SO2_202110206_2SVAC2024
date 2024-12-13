#ifndef _LINUX_MEMORY_SNAPSHOT_H
#define _LINUX_MEMORY_SNAPSHOT_H

#include <linux/types.h>

struct mem_snapshot {
    __u64 total_memory;   // Total de memoria física
    __u64 free_memory;    // Memoria libre
    __u64 total_swap;     // Total de swap
    __u64 free_swap;      // Swap libre
    __u64 active_pages;   // Páginas activas
    __u64 cached_pages;   // Páginas en caché
};

#define SYS_CAPTURE_MEMORY_SNAPSHOT 551

#endif /* _LINUX_MEMORY_SNAPSHOT_H */
