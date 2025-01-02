# <div align="center">Proyecto 3</div>
### <div align="center">USAC - Facultad de Ingeniería</div>
### <div align="center">Sistemas Operativos 2 - Sección A</div>
### <div align="center">Vacaciones Segundo Semestre 2024</div>
___

### <div align="center">Nombre: Julio Alejandro Zaldaña Ríos - Carnet: 202110206</div>
*<div align="center">Guatemala 2 de enero 2025</div>*

___

### **<div align="center"> Limitador de memoria para procesos en Linux </div>**

Para el Proyecto 3 del Curso de Sistemas Operativos, se realiza la implementación de un limitador de memoria para procesos, el cual asegura limitar los recursos de la computadora y mantenerlos bajos.
Es así que se ha planteado implementar un sistema de memoria que guarde los registros de los procesos que están limitados en una lista. El sistema consiste en la creación de una lista enlazada, la cual ayudará a realizar el control, que va a a monitorear que procesos deben ser limitados y cual es el valor máximo de memoria para cada uno de ellos.

___

### **<div align="center"> Lista Enlazada </div>**


Lista guardará el PID del proceso y la cantidad de memoria maxima que el proceso tiene que ser capaz de pedir de memoria.
Si el proceso excedió ya la cantidad de memoria que quiero añadirle.
Si le intento dar un PID que no existe?
Si quiero quitar el PID de la lista, para deslimitarlo.

___

### **<div align="center"> Creación de Syscalls </div>**

La creación de syscalls ha sido de vital importancia, ya que consiste en los funcionamientos más importantes de la lista manejadora de memoria de procesos.

Las syscalls se basan en el funcionamiento como si fuese un CRUD. 

C: Create - *Syscall 1: julioz_add_memory_limit*
R: Read - *Syscall 2: julioz_get_memory_limits*
U: Update - *Syscall 3: julioz_update_memory_limit*
D: Delete - *Syscall 4: julioz_remove_memory_limit*

#### Syscall 1: **julioz_add_memory_limit** (Limitación de un proceso)

* Solo para sudo

Agregar procesos a una lista.

#### Syscall 2: **julioz_get_memory_limits** (Obtención de una lista de procesos limitados)

* Para cualquier usuarios

#### Syscall 3: **julioz_update_memory_limit** (Actualización del límite de un proceso)

* Solo para sudo

#### Syscall 4: **julioz_remove_memory_limit** (Remover el límite de un proceso)

* Solo para sudo
___

### **<div align="center"> Pruebas </div>**

Si un proceso está limitado, y le quiero agregar más memoria, me tiene que tirar un error.
Interceptar las llamadas de asignación de memoria, malloc. 
Poder devolver errores, para tener un control total de los procesos.
Implementar el funcionamiento, si un proceso ya tiene memoria. Si le queremos agregar.

Guardar la lista de procesos limitados.
Array no se puede.
Tiene que ser una estructura de datos dinamica. 
El PID limitalo con 10 MB.




___ 

## **<div align="center">Habilidades Blandas</div>**

Se ha realizado una pequeña planificación de acuerdo el tiempo del proyecto, y se han añadido algunas notas sobre la experiencia al momento de desarrollar el proyecto.

### **<div align="center">Autogestión del Tiempo</div>**

| Fecha | Actividad  |
| - | - |
| 31 de diciembre | Implementación Lista Enlaza y Syscall 1, 2, 3 y 4 |
| 1 de enero | Rastreo de Malloc y Pruebas |
| 2 de enero | Pruebas |


### **<div align="center">Responsabilidad y Compromiso</div>**

Se debe de tomar en cuenta que para la correcta elaboración del proyecto,se necesita un tiempo considerable para investigar sobre los structs y funcionamientos generales de las herramientas a utilizar en el proyecto, al igual para poder ubicar, implementar y realizar pruebas respectivas de las funcionalidades solicitadas.

Se necesita un gran compromiso, para poder implementar, recompilar el kernel constantemente para hacer pruebas, verificar funcionalidades etc.

### **<div align="center">Errores Comunes y Soluciones</div>**

Al compilar el código de la SYSCALL TAMALLOC, me ha salido el siguiente error:

```bash
kernel/usac/tamalloc.c: In function ‘__do_sys_julioz_tamalloc’:
kernel/usac/tamalloc.c:23:12: error: too few arguments to function ‘do_mmap’
   23 |     addr = do_mmap(NULL, 0, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, 0);
      |            ^~~~~~~
In file included from ./include/linux/ring_buffer.h:5,
                 from ./include/linux/trace_events.h:6,
                 from ./include/trace/syscall.h:7,
                 from ./include/linux/syscalls.h:93,
                 from kernel/usac/tamalloc.c:2:
./include/linux/mm.h:3371:22: note: declared here
 3371 | extern unsigned long do_mmap(struct file *file, unsigned long addr,
```

**Causa:**

La función do_mmap requiere más argumentos de los que se han proporcionado. Sin embargo, después de verificar y realizar pruebas, se llegó a la conclusión que no se debería usar directamente do_mmap en una syscall, porque es una función interna del kernel. 


**Solución:**

Se provee la solución de utilizar en vez de do_mmap, utilizar la función **vm_mmap**, que es una capa de abstracción más sencilla para mapear memoria en syscalls. 

```c
unsigned long vm_mmap(struct file *file, unsigned long addr,
                      unsigned long len, unsigned long prot,
                      unsigned long flags, unsigned long pgoff);
```                  







### **<div align="center">Reflexión Personal</div>**

En general el segundo proyecto del laboratorio del curso, es muy interesante y constructivo; ya que se puede experimentar bastante en la temática de asignación de memoria de un sistema operativo, en este caso Linux. En este caso estudiar más a fondo el comportamiento de muchas funciones que alojan memoria como lo son calloc, malloc etc, poder implementar una variante, es de gran utilidad, ya que brinda una gran perspectiva de como funciona la repartición de memoria a los procesos de un sistema operativo. De igual manera, poder utilizar de estadísticas del sistema implementando syscalls para verificar el comportamiento del alojador creado *tamalloc*. 

El tiempo ha sido un factor importante y aprovecharlo es de vital importancia, para poder llegar a investigar, implementar funcionalidades, y realizar pruebas en el sistema operativo.

___

### **<div align="center"> E-grafía </div>**

- 

