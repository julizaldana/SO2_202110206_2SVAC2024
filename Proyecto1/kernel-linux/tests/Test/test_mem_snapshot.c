#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/kernel.h>

// Define el número de syscall según syscall_64.tbl
#ifndef SYS_CAPTURE_MEMORY_SNAPSHOT
#define SYS_CAPTURE_MEMORY_SNAPSHOT 551
#endif

struct mem_snapshot {
    unsigned long total_memory;
    unsigned long free_memory;
    unsigned long total_swap;
    unsigned long free_swap;
    unsigned long active_pages;
    unsigned long cached_pages;
};


int main() {
    struct mem_snapshot snapshot;
    long result = syscall(SYS_CAPTURE_MEMORY_SNAPSHOT, &snapshot);

    if (result == 0) {
        printf("Memory Snapshot:\n");
        printf("Total Memory: %lu KB\n", snapshot.total_memory);
        printf("Free Memory: %lu KB\n", snapshot.free_memory);
        printf("Total Swap: %lu KB\n", snapshot.total_swap);
        printf("Free Swap: %lu KB\n", snapshot.free_swap);
        printf("Active Pages: %lu KB\n", snapshot.active_pages);
        printf("Cached Pages: %lu KB\n", snapshot.cached_pages);
    } else {
        perror("syscall");
    }

    return 0;
}

