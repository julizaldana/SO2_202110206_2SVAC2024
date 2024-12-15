#ifndef _SYSCALL3_H
#define _SYSCALL3_H

#include <linux/types.h>

struct io_stats {
    __u64 bytes_read;
    __u64 bytes_written;
    __u64 cancel_count;  
    __u64 io_wait_time;;
    __u64 disk_io_time;
};

#define SYS_CAPTURE_MEMORY_SNAPSHOT 553

#endif /* _SYSCALL3_H */
