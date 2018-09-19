#include <stdio.h>  // printf
#include <string.h> // memset
#include <stdlib.h> // exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define SERVER "127.0.0.1"
#define BUFLEN 512  // Max length of buffer
#define PORT 8888   // The port on which to send data

#define QUEUE_SIZE 93     // Tamanho das filas dos roteadores
#define MESSAGE_SIZE 100  // Mensagens são limitadas a 100 caracteres

typedef struct{
  int id, port;
  int message_in, message_out;
  char ip[30];
}router;

typedef struct{
  int source, destination, type;    // Header
  char content[100];                // Mensagem
}package

typedef struct{
  int origin, destination;
  int cost;
}table;

int int main() {
  int tab_rot[N_ROT][N_ROT];

  return 0;
}
