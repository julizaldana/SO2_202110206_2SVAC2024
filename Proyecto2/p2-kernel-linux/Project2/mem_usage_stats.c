#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/kernel.h>

// Define el número de la syscall según syscall_64.tbl
#ifndef SYS_JULIOZ_GET_MEMORY_USAGE_STATS
#define SYS_JULIOZ_GET_MEMORY_USAGE_STATS 555
#endif

// Estructura que debe coincidir con la definida en la syscall
struct mem_usage_stats {
    int pid;               // PID del proceso
    unsigned long vmsize;  // Memoria reservada (VmSize)
    unsigned long vmrss;   // Memoria comprometida (VmRSS)
    int oom_score;         // Puntuación OOM
};

// Función para imprimir una línea de separación
void print_separator() {
    printf("+-------+----------------------+----------------------+-------------+\n");
}

// Función para imprimir el encabezado de la tabla
void print_header() {
    print_separator();
    printf("|  PID  | Reserved Memory  | Committed Memory  |  OOM Score  |\n");
    print_separator();
}

// Función para imprimir una fila de la tabla
void print_row(struct mem_usage_stats *stats) {
    printf("| %5d | %20lu | %20lu | %11d |\n",
           stats->pid, stats->vmsize, stats->vmrss, stats->oom_score);
}

int main(int argc, char *argv[]) {
    int pid;
    struct mem_usage_stats stats;

    // Solicitar el PID al usuario
    printf("Ingrese un PID (0 para todos los procesos): ");
    scanf("%d", &pid);

    if (pid == 0) {
        // Iterar sobre todos los procesos
        printf("\nEstadísticas de todos los procesos:\n");
        print_header();

        while (1) {
            long result = syscall(SYS_JULIOZ_GET_MEMORY_USAGE_STATS, pid, &stats);

            if (result < 0) {
                perror("syscall");
                break;
            }

            print_row(&stats);

            // Salir cuando no haya más procesos
            if (stats.pid == 0) {
                break;
            }
        }

        print_separator();
    } else {
        // Solicitar estadísticas de un proceso específico
        long result = syscall(SYS_JULIOZ_GET_MEMORY_USAGE_STATS, pid, &stats);

        if (result == 0) {
            printf("\nEstadísticas del proceso PID=%d:\n", pid);
            print_header();
            print_row(&stats);
            print_separator();
        } else {
            perror("syscall");
        }
    }

    return 0;
}
