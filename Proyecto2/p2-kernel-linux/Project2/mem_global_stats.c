#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/kernel.h>

#ifndef SYS_JULIOZ_GET_GLOBAL_MEMORY_USAGE_STATS
#define SYS_JULIOZ_GET_GLOBAL_MEMORY_USAGE_STATS 556
#endif

struct global_mem_stats {
    unsigned long total_reserved_mb;  // Memoria total reservada (VmSize) en MB
    unsigned long total_committed_mb; // Memoria total utilizada (VmRSS) en MB
};

int main() {
    struct global_mem_stats stats;

    // Llamar a la syscall
    if (syscall(SYS_JULIOZ_GET_GLOBAL_MEMORY_USAGE_STATS, &stats) < 0) {
        perror("syscall");
        return 1;
    }

    // Mostrar los resultados
    printf("Memoria total Reserved: %lu MB\n", stats.total_reserved_mb);
    printf("Memoria total Committed: %lu MB\n", stats.total_committed_mb);

    return 0;
}

