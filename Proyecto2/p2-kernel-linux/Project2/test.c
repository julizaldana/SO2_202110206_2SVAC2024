#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/kernel.h>
#include <stdlib.h>

#ifndef SYS_JULIOZ_GET_MEMORY_USAGE_STATS
#define SYS_JULIOZ_GET_MEMORY_USAGE_STATS 555
#endif

struct mem_usage_stats {
    int pid;
    unsigned long vmsize;  // Memoria reservada (VmSize) en KB
    unsigned long vmrss;   // Memoria comprometida (VmRSS) en KB
    int oom_score;         // Puntuación OOM
};

int main(int argc, char *argv[]) {
    int pid;
    size_t max_entries = 256; // Máximo número de procesos a listar
    struct mem_usage_stats *buffer;

    printf("Ingrese un PID (0 para todos los procesos): ");
    scanf("%d", &pid);

    buffer = malloc(max_entries * sizeof(struct mem_usage_stats));
    if (!buffer) {
        perror("malloc");
        return 1;
    }

    long result = syscall(SYS_JULIOZ_GET_MEMORY_USAGE_STATS, pid, buffer, max_entries);

    if (result < 0) {
        perror("syscall");
    } else {
        printf("\nInformación de procesos:\n");
        printf("+-------+----------------+----------------+-----------+\n");
        printf("|  PID  | Reserved (KB)  | Committed (KB) | OOM Score |\n");


        for (long i = 0; i < result; i++) {
            printf("+-------+----------------+----------------+-----------+\n");
            printf("| %5d | %14lu | %14lu | %9d |\n",
                   buffer[i].pid, buffer[i].vmsize, buffer[i].vmrss, buffer[i].oom_score);
            printf("+-------+----------------+----------------+-----------+\n");

        }

    }

    free(buffer);
    return 0;
}

