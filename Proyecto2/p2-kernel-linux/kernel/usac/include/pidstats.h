#ifndef _MEM_USAGE_STATS_H
#define _MEM_USAGE_STATS_H

#include <linux/types.h>

struct mem_usage_stats {
    int pid;               // PID del proceso
    __u64 vmsize;          // Memoria reservada (VmSize)
    __u64 vmrss;           // Memoria comprometida (VmRSS)
    int oom_score;         // Puntuación OOM
};

#define SYS_JULIOZ_GET_MEMORY_USAGE_STATS 555

#endif /* _MEM_USAGE_STATS_H */
