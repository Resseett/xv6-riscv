1. Funcionamiento de las nuevas llamadas al sistema

En esta tarea se implementaron dos nuevas llamadas al sistema en xv6-riscv.

1.1. int mrdprotect(void*, int)

Esta llamada al sistema recibe como argumentos una dirección virtual addr y un entero len que indica cuántas páginas consecutivas se desean proteger.

Su propósito es marcar la región de memoria como “no legible” por el proceso, limpiando el bit PTE_R en cada entrada de tabla de páginas (PTE) involucrada, pero manteniendo intactos los demás bits del PTE.



1.2. int munrdprotect(void*, int)

Esta llamada al sistema tiene la función inversa: vuelve a habilitar el permiso de lectura en el mismo rango de páginas.

Recibe los mismos parámetros y recorre los PTE asociados a la región, restaurando el bit PTE_R en cada PTE de usuario válido. Como en el caso anterior, no se alteran los otros bits del PTE.



Manejo de errores de ambas funciones

Ambas llamadas al sistema devuelven -1 si ocurre alguna de las siguientes situaciones:

La dirección addr no está alineada al tamaño de página (PGSIZE).

El valor len es menor o igual a 0.

Alguna de las direcciones del rango no corresponde al espacio de usuario del proceso.

Alguna página del rango no tiene el bit PTE_V activo, es decir, no está mapeada.

La operación implicaría modificar páginas que pertenecen al kernel (sin PTE_U).

En caso de éxito, tanto mrdprotect() como munrdprotect() retornan 0.

2. Explicación de las modificaciones realizadas

Para implementar estas dos nuevas llamadas al sistema fue necesario modificar varios archivos tanto en el núcleo como en el espacio de usuario de xv6.

2.1. Archivo kernel/syscall.h

En este archivo se agregaron las constantes para los nuevos números de syscall:

#define SYS_mrdprotect   24
#define SYS_munrdprotect 25

Esto permite que el kernel identifique a cada llamada al sistema con un número único cuando es invocada desde el espacio de usuario.

2.2. Archivo kernel/syscall.c

En syscall.c se realizaron dos tipos de cambios. Primero, se declararon las funciones de las nuevas syscalls:

extern uint64 sys_mrdprotect(void);
extern uint64 sys_munrdprotect(void);


Luego, se agregaron las entradas correspondientes en el arreglo syscalls[], asociando los números definidos en syscall.h con las funciones que se ejecutan en el kernel:

[SYS_mrdprotect]  sys_mrdprotect,
[SYS_munrdprotect] sys_munrdprotect,


De esta forma, cuando un proceso de usuario invoca mrdprotect() o munrdprotect(), el kernel sabe qué función ejecutar.

2.3. Archivo kernel/sysproc.c

En este archivo se implementan los wrappers de las nuevas llamadas al sistema

Estas funciones son las encargadas de recuperar los argumentos entregados por el proceso de usuario, verificar condiciones básicas (alineamiento, longitud mayor que cero), llamar a las funciones de bajo nivel que realmente modifican los PTE en vm.c, pasando la tabla de páginas del proceso (myproc()->pagetable).

2.4. Archivo kernel/defs.h

En defs.h se declararon los prototipos de las nuevas funciones que operan directamente sobre la tabla de páginas.

Esto permite que dichas funciones se puedan llamar desde otros archivos del kernel.

2.5. Archivo kernel/vm.c

Este archivo contiene la lógica de memoria virtual. Aquí se implementaron las funciones que recorren las PTE y modifican el bit de lectura:

En mrdprotect() se utiliza walk(pagetable, addr, 0) para obtener el PTE de cada página del rango, verificando que sea válido (PTE_V) y de usuario (PTE_U). Si alguna página no cumple estas condiciones, la función retorna -1. En caso contrario, se limpia el bit PTE_R:

En munrdprotect() se recorre el mismo rango de PTE y se restaura el permiso de lectura:

Al finalizar la modificación de los PTE se invoca sfence_vma() para invalidar la TLB y asegurarse de que la CPU vea los nuevos permisos de página.

2.6. Archivo user/user.h

En el archivo de cabecera del espacio de usuario se añadieron las declaraciones de las nuevas funciones. Esto permite que cualquier programa de usuario pueda llamar a estas funciones y compilar sin errores.

2.7. Archivo user/usys.pl

En el script usys.pl se agregaron las entradas:

entry("mrdprotect");
entry("munrdprotect");

2.8. Archivo Makefile

Finalmente, en el Makefile se agregó el nuevo programa de prueba a la lista UPROGS.

De esta forma, el ejecutable se incluye en la imagen de usuario y puede ser ejecutado desde la shell de xv6.

3. Dificultades encontradas y su resolución

Durante la implementación de estas llamadas al sistema se presentaron varias dificultades, tanto de compilación como de diseño. A continuación se destacan las más relevantes.

3.1. Uso incorrecto de argaddr() y argint()

Inicialmente se intentó validar el retorno de las funciones argaddr() y argint() de la siguiente forma:

if (argaddr(0, &addr) < 0 || argint(1, &len) < 0)
  return -1;

En donde estas secciones generaban un error y para solucionarlo se modificaron las llmadas a estas funciones para cargar los argumentos y luego realizar las validaciones de manera explícita sobre las variables resultantes:

argaddr(0, &addr);
argint(1, &len);

if (len <= 0 || (addr % PGSIZE) != 0)
  return -1;

De esta forma se evita el uso incorrecto del valor de retorno y se cumple con la lógica de validación requerida.

3.2. Diferencia entre direcciones de usuario y del kernel

Otra dificultad fue asegurarse de no modificar páginas del kernel. Para esto se verificó que en cada PTE estuviera activo el bit PTE_U. Si este bit no está presente, la página pertenece al kernel y la función debe fallar retornando -1. Esto protege la memoria del sistema operativo de modificaciones no deseadas por parte de procesos de usuario.

3.3. Manejo de PTE no válidos

También fue necesario manejar el caso en que alguna de las páginas del rango no esté mapeada, es decir, su PTE no tenga el bit PTE_V activado. En ese caso, la función retorna -1 indicando error, ya que no se puede cambiar permisos de una página inexistente.
