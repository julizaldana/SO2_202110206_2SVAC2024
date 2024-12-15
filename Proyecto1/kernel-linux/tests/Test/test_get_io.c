#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/kernel.h>

// Define el número de la syscall según syscall_64.tbl
#ifndef SYS_GET_IO_THROTTLE
#define SYS_GET_IO_THROTTLE 553 
#endif

// Estructura que debe coincidir con la definida en la syscall
struct io_stats {
    unsigned long long bytes_read;
    unsigned long long bytes_written;
    unsigned long long cancel_count;
    unsigned long long io_wait_time;
    unsigned long long disk_io_time;
};

int main() {
    struct io_stats stats;

    // Llamar a la syscall
    long result = syscall(SYS_GET_IO_THROTTLE, &stats);

    // Verificar el resultado
    if (result == 0) {
        printf("I/O Throttle Stats:\n");
        printf("Bytes Read: %llu\n", stats.bytes_read);
        printf("Bytes Written: %llu\n", stats.bytes_written);
        printf("Cancel Count: %llu\n", stats.cancel_count);
        printf("I/O Wait Time: %llu\n", stats.io_wait_time);
        printf("Disk I/O Time: %llu\n", stats.disk_io_time);
    } else {
        perror("syscall");
    }

    return 0;
}

