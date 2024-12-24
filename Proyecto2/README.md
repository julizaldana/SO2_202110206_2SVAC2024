# <div align="center">Proyecto 2</div>
### <div align="center">USAC - Facultad de Ingeniería</div>
### <div align="center">Sistemas Operativos 2 - Sección A</div>
### <div align="center">Vacaciones Segundo Semestre 2024</div>
___

### <div align="center">Nombre: Julio Alejandro Zaldaña Ríos - Carnet: 202110206</div>
*<div align="center">Guatemala 26 de diciembre 2024</div>*

___

### **<div align="center"> Asignador de memoria con lazy-zeroing en Linux </div>**

Para el Proyecto 2 del Curso de Sistemas Operativos, se realiza la implementación de un asignador de memoria el cual asegure la inicialización de memoria en 0 sin marcar toda su memoria como utilizada
inmediatamente. Se ha construido una variante con el nombre de **tamalloc.** 

Para comprender un poco sobre la implementación del funcionamiento de tamalloc; se deberá entender algunos conceptos y principios de asignación de memoria.


* **malloc:**
Devuelve un puntero. Marca la memoria como Reservada.

void *ptr = malloc (n * size_of_elements);
memset(ptr, 0 , n * size_of_elements);

![alt text](./images/malloc.png)

* **calloc**
Hace uso de memset, la memoria que se pide, se marcan en 0.
Ya no es reservada sino que es memoria commited.

![alt text](./images/calloc.png)

* **tamalloc**
Tiene que quedar en reserved, pero si inicializado en 0.

![alt text](./images/tamalloc.png)

* mmap

* new

* delete

* memset - Ver donde está implementado, y quitarlo lo de commited.

Cualquier proceso utiliza malloc, para poder asignar memoria.
Dar mas memoria de la que tiene. 

OverCommit-Dar mas memoria.
OOM-killer. 

| | malloc | calloc | tamalloc |
| - | - | - | - |
| init en 0 |  ❌ | ✔  | ✔ |
| VmSize Inmediato |  ✔ | ✔ | ✔ |  
| VmRSS No Inmediato | ✔   | ❌  | ✔ |


Vmsize: Devuelve cuanta memoria ha sido alocada (reservada solicitada) por un proceso.

VmRSS: Nos brinda la cantidad de memoria que reside en RAM, de la cual esta siendo activamente usada.


Lazy-loading
Lazy-init
Lazy-zeroing
Mapping de memoria virtual:
- MAP_NORESERVE
- MAP_PRIVATE
- MAP_ANONYMOUS

Guiarme de kzalloc, calloc

El alojador de memoria Tamalloc tendrá como objetivo:

1. Toma como argumento la cantidad de memoria a alojar
2. Devolver un puntero como cualquier otro alojador.
3. Inicializar en el espacio 0.
4. No marcar la memoria como utilizada (relacionado al OOM)

Estadísticas de memoria por cada proceso.
1) Reserved Memory
2) Commited Memory
3) OOM Score

Estadísticas de uso de memoria. (Mostrado en MB y en porcentaje de la memoria TOTAL) 

1) Memoria TOTAL que ha sido reservada.
2) Utilización (Commited / No Reserved)

Total del sistema
Total reservada
Total commited
___

### **<div align="center"> Desarrollo de Tamalloc </div>**

Tamalloc devolverá un puntero.

void *ptr tamalloc(size_t size)

void * mem_pointer = malloc (size)

//si mem_pointer es un nullptr, devolver nullptr)

int res = memset_modificado(0,mem_pointer,size)

//si res no es 0, devolver res

return mem_pointer

![alt text](image.png)

int memset_modificado(char filler, void* starting_address, size_t size (cantidad de bytes que se van a llenar de 0s))

int res = memset_modificado(filler, mem_pointer, size)

int memset_modificado(char filler, void* starting_adress, size_t size)


___

### **<div align="center"> Creación de Syscalls </div>**

1. **Tamalloc** (Función para asignación de memoria)




2. **VM_stats** (Consulta de Estadísticas de Procesos)




3. **Mem_Stats**




Suma de MemFree, Bufer y Cached - Reservada o no utilizada
 Total de Memoria - Reservada - Utilizada.


___

### **<div align="center"> Pruebas y Estadísticas </div>**

Se obtiene en una tabla las estadísticas de memoria por cada proceso.

1) Reserved Memory
2) Commited Memory
3) OOM Score

en cat/proc/num/score

Modulo en C, 
Lo ideal sería que esté en el Kernel.

___ 

## **<div align="center">Habilidades Blandas</div>**

Se ha realizado una pequeña planificación de acuerdo el tiempo del proyecto, y se han añadido algunas notas sobre la experiencia al momento de desarrollar el proyecto.

### **<div align="center">Autogestión del Tiempo</div>**

| Fecha | Actividad  |
| - | - |
| 21 de diciembre | Investigación conceptos de Asignación de Memoria|
| 22 de diciembre | Implementación Tamalloc |
| 23 de diciembre | Implementación Tamalloc, 1er syscall y prueba |
| 24 de diciembre | Creación de 2da syscall y 3era syscall |
| 25 de diciembre | Prueba de 3era syscall y validaciones |

### **<div align="center">Responsabilidad y Compromiso</div>**



### **<div align="center">Errores Comunes y Soluciones</div>**

Se han obtenido algunos errores comunes como: 

```bash
```

Causa:

Solución:

Tiene que aceptar el PID de un proceso.

SYSCALL_DEFINE1(julioz_get_allocators_statistics, int pid)
(

Si le pasamos el PID 0 que devuelve el 0

Pero si le pasamos un PID != 0, que devuelve ese solo proceso.

    if (pid == 0){


    }


)

tamalloc da un bloque de memoria.

Cuando ejecutamos malloc.

malloc(70kb)
0x7831 - 
70kb

VM RSS tiene que ir subiendo de a pocos .
Vm Size se queda igual

Mejor hacer una syscall.
Poner el PID.

grep.

Manden.
TAMALLOC ES PARA ASIGNACIÓN DE MEMORIA.

(int*) ptr = malloc(size)
memset(0, ptr, size)

//evitar 

Los procesos normales usan malloc.

MemTotal ya está.
Necesitamos el reservado y el commited.

MAP_NORESERVE, MAP_PRIVATE y MAP_ANONYMOUS.

MEMSET no va a estar en TAMALLOC

MALLOC No inicializa en 0 la memoria.

Cambiar malloc con calloc.




#define SYSCALL_TAMALLOC_ID 555
#define ALLOC_SIZE
int main() {
     


     (void*) memory_ptr = syscall(SYSCALL_TAMALLOC_ID, ALLOC_SIZE);

     if (!memory_ptr) {
        printf("Error! No obtuvimos un puntero valido")
        //TODO poner p_error()
     }
     

     // TODO read("Presione ENTER para empezar la lectura del bloque de memoria")
}
void* 

RSS - Bajo (2MB)
VSS poco mas de 100 MB

Inicializar cada pagina 

for (int i=0; i<ALLOC_SIZE; ++i) {
    if (i%10 == 0) {
        printf("Se leyeron 50 bytes, esperando 1 segundo\n");
    }
}

### **<div align="center">Reflexión Personal</div>**


___

### **<div align="center"> E-grafía </div>**
