#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define QUEUE_SIZE 93     // Tamanho das filas dos roteadores
#define MESSAGE_SIZE 100  // Mensagens são limitadas a 100 caracteres
#define N_ROT 7           // Número de roteadores

typedef struct{           // Estrutura dos roteadores
  int id, port;
  int message_in, message_out;
  char ip[30];
}router;

typedef struct{                     // Estrutura dos pacotes
  int source, destination, type;    // Header
  char content[100];                // Mensagem
}package;

typedef struct{                     // Estrutura das tabelas de roteamentos
  int origin, destination;
  int cost;
  int path[N_ROT];
}table;
