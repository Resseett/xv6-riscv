#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    int pid;
    int my_pid = getpid();
    printf("El proceso actual tiene un PID: %d\n", my_pid);

    printf("Hijo dice: Mi padre tiene PID %d\n", getppid());
    
    printf("Probando getancestor(0):\n");
    pid = getancestor(0);
    printf("PID de mi mismo: %d\n", pid);

    printf("\nProbando getancestor(1):\n");
    pid = getancestor(1);
    printf("PID de mi padre: %d\n", pid);

    printf("\nProbando getancestor(2):\n");
    pid = getancestor(2);
    printf("PID de mi abuelo: %d\n", pid);
   
    printf("\nProbando getancestor(8):\n");
    pid = getancestor(8);
    printf("PID del ancestro 8: %d\n", pid);
    exit(0);
}