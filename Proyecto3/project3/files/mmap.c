//interceptando malloc

//chequea si se le deniega o se le acepta la solicitud de memoria.
//intercepción de brk

if (!is_request_accepted(current->pid, newbrk - oldbrk)) {
    goto out;
}

//intercepcion de mmap
if (!is_request_accepted(current->pid, len)) {
    retval = -ENOMEN;
    goto out_fput; 
}