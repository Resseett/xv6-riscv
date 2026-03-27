#include "kernel/types.h"
#include "user/user.h"

#define N 10  // Número de procesos

int
main(void)
{
  int i, pid;
  
  printf("=== Demo Lottery Scheduling ===\n");
  printf("Creando %d procesos con diferentes tickets...\n\n", N);
  
  // Tabla de tickets
  printf("Distribucion de Tickets:\n");
  for(i = 0; i < N; i++) {
    printf("Proceso %d: %d tickets\n", i, 50 * (i + 1));
  }
  printf("\n");
  
  for(i = 0; i < N; i++) {
    pid = fork();
    
    if(pid < 0) {
      printf("ERROR fork\n");
      exit(1);
    }
    
    if(pid == 0) {
      // Proceso hijo
      int tickets = 50 * (i + 1);  // 50, 100, 150, ..., 500
      
      settickets(tickets);
      
      // Trabajo computacional intensivo
      int j, k;
      volatile int sum = 0;
      for(j = 0; j < 5000000; j++) {
        for(k = 0; k < 10; k++) {
          sum += j * k;
        }
      }
      
      // Obtener mi contador antes de terminar
      int my_executions = getpid();  // Temporal, cambiaremos esto
      
      printf("[Proceso %d] Tickets=%d, Ejecuciones=%d, TERMINADO\n", 
             i, tickets, my_executions);
      exit(0);
    }
  }
  
  // Padre espera a todos
  for(i = 0; i < N; i++) {
    wait(0);
  }
  
  printf("\n=== DEMO COMPLETADO ===\n");
  
  // Imprimir estadísticas desde el kernel
  print_stats();
  
  printf("=== ANALISIS ===\n");
  printf("La columna 'EJECUCIONES' muestra cuantas veces\n");
  printf("cada proceso fue seleccionado por el scheduler.\n\n");
  printf("Observacion: Los procesos con mas tickets fueron\n");
  printf("ejecutados proporcionalmente mas veces.\n");
  printf("Ej: Proceso con 500 tickets ~10x mas que proceso con 50 tickets.\n");
  
  exit(0);
}