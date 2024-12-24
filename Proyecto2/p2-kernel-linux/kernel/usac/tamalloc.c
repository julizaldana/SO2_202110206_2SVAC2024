#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/uaccess.h>

SYSCALL_DEFINE1(julioz_tamalloc, size_t, size)
{
    unsigned long addr;
    size_t aligned_size;

    if (size == 0) {
        return -EINVAL; // Error: Tamaño inválido
    }

    // Alinear el tamaño a la página más cercana
    aligned_size = PAGE_ALIGN(size);
    if (!aligned_size) {
        return -ENOMEM; // Error: No se pudo alinear el tamaño
    }

    //MAP_PRIVATE: Con Cow (Area privada, no compartida) 
    //MAP_ANONYMOUS: El contenido del mapeo anonimo de memoria sería inicializado en 0.
    //MAP_NORESERVE: No reserva espacio swap / memoria(fisica). Para evitar buffer overflows.

    // Mapear memoria usando vm_mmap (MAPEO VIRTUAL)
    addr = vm_mmap(NULL, 0, aligned_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, 0);

    if (IS_ERR_VALUE(addr)) {
        printk(KERN_ERR "ERROR AL MAPEAR MEMORIA VIRTUAL CON TAMALLOC");
        return addr; // Retornar el error directamente
    }

    return addr; // Retornar la dirección mapeada
}
