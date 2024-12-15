#include <stdio.h>
#include <time.h>
#include <string.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>  // Para open()

#define SYSCALL_TRACK 552

struct syscall_usage {
    unsigned long count;
    struct timespec last_called;
};

#define MAX_SYSCALLS 1024

// Se pueden mapear los syscalls a nombres
const char *syscall_names[MAX_SYSCALLS] = {
    [0] = "read",
    [1] = "write",
    [2] = "open",
    [57] = "fork",
};

int main() {

    struct syscall_usage stats[MAX_SYSCALLS];
    memset(stats, 0, sizeof(stats));

    //pruebas
    char buffer[16];
    write(STDOUT_FILENO, "Test Syscall write: ", 19);
    read(STDIN_FILENO, buffer, sizeof(buffer));
    fork();
    open("testfile.txt", O_CREAT | O_WRONLY, 0644);


    if (syscall(SYSCALL_TRACK, stats) < 0) {
        perror("syscall");
        return 1;
    }


    printf("\nSyscall Tracking Results:\n");
    for (int i = 0; i < MAX_SYSCALLS; i++) {
        if (syscall_names[i] && stats[i].count > 0) { 
            printf("Syscall (%d: %s): Count=%lu, Last Called=%ld.%09ld\n",
                i,                            // Número del syscall
                syscall_names[i],             // Nombre del syscall
                stats[i].count,               // Conteo de invocaciones
                stats[i].last_called.tv_sec,  // Tiempo en segundos
                stats[i].last_called.tv_nsec  // Tiempo en nanosegundos
            );
        }
    }

    return 0;
}

