#include <stdio.h>  // printf
#include <string.h> // memset
#include <stdlib.h> // exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define QUEUE_SIZE 93     // Tamanho das filas dos roteadores
#define MESSAGE_SIZE 100  // Mensagens são limitadas a 100 caracteres
#define N_ROT 7           // Número de roteadores

typedef struct{
  int id, port;
  int message_in, message_out;
  char ip[30];
}router;

typedef struct{
  int source, destination, type;    // Header
  char content[100];                // Mensagem
}package;

typedef struct{
  int origin, destination;
  int cost;
  int path[N_ROT];
}table;

int main(){
  int tab_rot[N_ROT][N_ROT];

  return 0;
}
