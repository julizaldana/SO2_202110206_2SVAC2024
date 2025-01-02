#ifndef _LINUX_CRUD_MEM_LIMIT_H
#define _LINUX_CRUD_MEM_LIMIT_H

bool is_request_accepted(pid_t pid, size_t requested_memory);

#endif /*   _LINUX_CRUD_MEM_LIMIT_H*/