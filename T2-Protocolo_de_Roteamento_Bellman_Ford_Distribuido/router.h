#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define QUEUE_SIZE 93     // Tamanho das filas dos roteadores
#define MESSAGE_SIZE 100  // Mensagens são limitadas a 100 caracteres
#define MAX_ROT 100

typedef struct{                                             // Estrutura dos roteadores
  int id, port;                                             // ID e porta do roteador
  Package message_in[QUEUE_SIZE], message_out[QUEUE_SIZE];  // Filas do roteador
  char ip[30];
}Router;

typedef struct{                     // Estrutura dos pacotes
  int msg_id;
  int source, destination, type;    // Header (Origem, Destino e tipo da mensagem)
  char content[100];                // Mensagem
}Package;

typedef struct{                     // Estrutura das tabelas de roteamentos
  int cost;
  int next;
}Table;
