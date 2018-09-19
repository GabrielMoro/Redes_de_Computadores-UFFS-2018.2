#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define SERVER "127.0.0.1"
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to send data

typedef struct{
  int id, port, message_in, message_out;
  char ip[30];
}router;

int int main() {
  int tab_rot[N_ROT][N_ROT];

  return 0;
}
