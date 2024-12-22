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

* kmalloc

* malloc

* calloc

* mmap

* new

* delete


| | malloc | calloc | tamalloc |
| - | - | - | - |
| init en 0 |  ❌ | ✔  | ✔ |
| VmSize Inm |  ✔ | ✔ | ✔ |  
| VmRSS No Inm | ✔   | ❌  | ✔ |

Lazy-loading
Lazy-init
Lazy-zeroing
Mapping de memoria virtual:
- MAP_NORESERVE
- MAP_PRIVATE
- MAP_ANONYMOUS



___

### **<div align="center"> Desarrollo de Tamalloc </div>**





___

### **<div align="center"> Creación de Syscalls </div>**



___

### **<div align="center"> Pruebas y Estadísticas </div>**



___

### **<div align="center"> E-grafía </div>**
