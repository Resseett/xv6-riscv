#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    int pid;

    printf("Probando getancestor(0):\n");
    pid = getancestor(0);
    printf("PID de mi mismo: %d\n", pid);

    printf("\nProbando getancestor(1):\n");
    pid = getancestor(1);
    printf("PID de mi padre: %d\n", pid);

    printf("\nProbando getancestor(1):\n");
    pid = getancestor(2);
    printf("PID de mi padre: %d\n", pid);

    printf("\nCreando un proceso hijo para probar getancestor(2):\n");
    int child_pid = fork();
    if (child_pid == 0) {
        int grandparent_pid = getancestor(2);
        int my_pid = getpid(); // Usar getpid() para obtener nuestro propio PID
        printf("Hijo: Mi PID es %d. El PID de mi abuelo (getancestor(2)) es: %d\n", my_pid, grandparent_pid);
        exit(0);
    } else {
        // Proceso padre
        wait(0);
        printf("\nPadre: Mi PID es %d. Mi hijo ha terminado.\n", getpid()); // Usar getppid() para obtener ID del padre
    }

    exit(0);
}