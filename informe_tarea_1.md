Implementación de Llamadas al Sistema en xv6

1. Funcionamiento de las Nuevas Llamadas al Sistema

Se han implementado dos nuevas llamadas al sistema en el sistema operativo xv6.

1.1. int getppid(void)
Esta llamada al sistema no recibe ningún argumento y su proposito es devolver el PID del proceso padre del proceso que la invoca (proceso hijo (PID) -> proceso padre (PPID)). Sirve para entender las modificaciones que se pueden hacer al nucleo de xv6.

* Comportamiento esperado: Dependiendo de cuando se llame se obtiene el proceso padre del actual, es decir, mientras no se haga un fork para crear un hijo siempre deberia dar 2, esto se debe a que no importa cuantas veces se llame a la funcion getpid (PID del proceso actual), lo unico que se hace es crear mas hijos para el padre, entonces el proceso actual puede ser 10 pero el proceso padre seguira siendo 2 porque del 3 al 9 todos son hijos. 

1.2. int getancestor(int n)
Esta llamada al sistema recibe un número entero como argumento y retorna el PID del ancestro que se encuentra en el nivel que se indica con el entero ingresado.

* getancestor(0): Retorna el PID del mismo proceso que invoca la función (El proceso actual).
* getancestor(1): Retorna el PID del padre del proceso actual.
* getancestor(2): Retorna el PID del abuelo del proceso actual.
* Manejo de errores: Si el número que se ingresa es mayor que la cantidad de ancestros que existen hasta el minuto (por ejemplo, un proceso que tiene solo hasta un abuelo e ingreso getancestor(8)), la función debe retornar -1.

----------------------------------------------------------------------

2. Explicación de las Modificaciones Realizadas

Para implementar estas dos llamadas al sistema, se modificaron y crearon archivos tanto en el núcleo (kernel/) como en el espacio de usuario (user/).


Archivo: kernel/syscall.h
Modificaciones realizadas: Se modificó con la adición de los identificadores SYS_getppid y SYS_getancestor con valores numéricos únicos.
Motivo de la modificación: El núcleo necesita un número para reconocer y enrutar las nuevas llamadas al sistema.

Archivo: kernel/syscall.c
Modificaciones realizadas: Se modificó con las declaraciones extern y la adición de sys_getppid y sys_getancestor a la tabla syscalls[].
Motivo de la modificación: El handler de llamadas al sistema necesita mapear los números de syscall.h con sus funciones de implementación correspondientes en el kernel.

Archivo: kernel/sysproc.c
Modificaciones realizadas: Se modificó con la implementación de las funciones sys_getppid() y sys_getancestor().
Motivo de la modificación: Estas funciones contienen la lógica central para acceder a la estructura proc y obtener el PID del padre o navegar la cadena de ancestros hasta el nivel solicitado.

Archivo: user/user.h
Modificaciones realizadas: Se modificó con la declaración de los prototipos int getppid(void); e int getancestor(int);.
Motivo de la modificación: Los programas de usuario necesitan conocer la firma de las funciones para poder compilar sin errores al llamarlas.

Archivos: user/yosoytupadre.c y user/ancestortest.c
Modificaciones realizadas: Se crearon con el código de prueba que llama a las nuevas funciones.
Motivo de la modificación: Es un requisito de la tarea validar y demostrar el correcto funcionamiento de getppid() y getancestor().

Archivo: Makefile
Modificaciones realizadas: Se modificó la variable UPROGS añadiendo _yosoytupadre y _ancestortest.
Motivo de la modificación: Se requiere compilar, enlazar e incluir los nuevos programas de prueba en la imagen final del sistema operativo.

----------------------------------------------------------------------

3. Dificultades Encontradas y su Resolución

La principal dificultad encontrada durante la implementación del getancestor() fue un error de compilación relacionado con el manejo de argumentos.

Dificultad: Error de compilación en sys_getancestor()

Al implementar la lógica para obtener el argumento n, se intentó comprobar el valor de retorno de la función argint() de la siguiente manera:

if(argint(0, &n) < 0) 
  return -1;

* Error generado por make: void value not ignored as it ought to be
* Causa: La función argint() en xv6 está definida para tener un tipo de retorno void (no retorna ningún valor), pero se estaba utilizando su resultado en una comparación (< 0). Esto indica un mal uso de la API interna del sistema operativo.
* Resolución: Se eliminó la comprobación del valor de retorno de argint(). La función simplemente se llama para cargar el argumento en la variable n, y luego se verifica la validez de n de forma independiente.
