#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <time.h>

#define SYS_TAMALLOC 554

int main() {
    printf("Program for tamalloc PID: %d\n", getpid());

    printf("Press ENTER to allocate memory using tamalloc...\n");
    getchar();

    size_t total_size = 10 * 1024 * 1024; // 10 MB

    // Llamar a la syscall tamalloc
    char *buffer = (char *)syscall(SYS_TAMALLOC, total_size);
    if ((long)buffer < 0) {
        perror("tamalloc failed");
        return 1;
    }
    printf("Allocated 10MB of memory using tamalloc at address: %p\n", buffer);

    printf("Press ENTER to start verifying memory initialization...\n");
    getchar();

    // Verificar que la memoria esté inicializada en 0
    for (size_t i = 0; i < total_size; i++) {
        if (buffer[i] != 0) {
            printf("ERROR: Memory at byte %zu was not initialized to 0\n", i);
            return 1;
        }
    }

    printf("Memory verified to be zero-initialized. Press ENTER to modify memory...\n");
    getchar();

    // Escribir datos aleatorios para activar CoW
    srand(time(NULL));
    for (size_t i = 0; i < total_size; i++) {
        buffer[i] = 'A' + (rand() % 26); // Escribir letras aleatorias
        if (i % (1024 * 1024) == 0 && i > 0) { // Cada 1 MB
            printf("Modified %zu MB of memory...\n", i / (1024 * 1024));
            sleep(1);
        }
    }

    printf("Memory successfully modified. Press ENTER to exit.\n");
    getchar();

    return 0;
}

