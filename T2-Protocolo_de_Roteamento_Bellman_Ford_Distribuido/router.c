#include "router.h"

Router router;
Table r_table[MAX_ROT];                                      // Tabela de roteamento
Neighbors n_table[MAX_ROT];
Dist_Vector dv_table[MAX_ROT];
// Package message_in[QUEUE_SIZE], message_out[QUEUE_SIZE];  // Filas do roteador

struct sockaddr_in si_me, si_other;

pthread_t receive_message, send_message, process_vector;

void die(char *s){
  perror(s);
  exit(1);
}

// void *send_message(Package msg){
//   int slen = sizeof(si_other);
//
//   while(1){
//     if(msg.type == DIST_VECTOR){
//
//     }
//   }
// }

// void *process_vector(Package msg){
//
// }

// void *receive_message(void *n){
//   int slen = sizeof(si_other);
//
//   while(1){
//
//   }
// }

void create_message(){
  int destination;
  Package msg;

  do{
    printf("\nDigite o roteador de destino: ");
    scanf("%d", &destination);
    if(destination < 0 || destination >= MAX_ROT)
      printf("Roteador informado não existe!\n\n");
  }while(destination < 0 || destination >= MAX_ROT);
  printf("Escreva a mensagem a ser enviada para %d:\n", destination);
  getchar();

  fgets(msg.content, MESSAGE_SIZE, stdin);

  msg.source = id;
  msg.destination = destination;
  msg.type = MESSAGE;


}

void init_topology(int r_id){
  FILE *links = fopen("enlaces.config", "r");
  FILE *config = fopen("roteador.config", "r");
  int x, y, w;
  int id, port;
  char ip[30];

  if(links && config){
    for (int i = 0; fscanf(links, "%d %d %d", &x, &y, &w) != EOF; i++){
      for(int i = 0; fscanf(config, "%d %d %s", &id, &port, ip) != EOF; i++){
        if(r_id == x){
          if(id == y){
            n_table[y].port = port;
            n_table[y].ip = ip;
            r_table[y].next = y;
            r_table[y].cost = w;
            dv_table[r_id].cost[y] = w;
            break;
          }
        }
        if(r_id == y){
          if(id == x){
            n_table[x].port = port;
            n_table[x].ip = ip;
            r_table[x].next = x;
            r_table[x].cost = w;
            dv_table[r_id].cost[x] = w;
            break;
          }
        }
      }
    }
  }
}

void router_config(int r_id){// Configura o roteador com base no arquivo router.config
  FILE *config = fopen("roteador.config", "r");

  if(config){
    int id, port;
    char ip[30];
    for(int i = 0; fscanf(config, "%d %d %s", &id, &port, ip) != EOF; i++)
      if(id != r_id)
        break;
      else{
        router.id = id;
        router.port = port;
        router.ip = ip;

        printf("ID: %d\n Porta: %d\n IP: %s\n", router.id, router.port, router.ip);
      }
  }else
    die("Erro ao configurar o roteador\n");
  fclose("config");

  if((sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    die("Erro ao criar Socket\n");

  memset((char *) &si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(router[r_ID].port);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(sckt, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
    die("Erro ao dar bind!");
}

int toint(char *str){// Converte de string/char para int // Função da internet
  int ans;
  ans = 0;
  for(int i = strlen(str) - 1, pot = 1; i >= 0; i--, pot *= 10)
    ans += pot * (str[i] - '0');
  return ans;
}

int main(int argc, char *argv[]){
  int tab_rot[MAX_ROT];
  int opt, aux;

  if(argc < 2)
    die("Insira o ID do roteador!\n");
  else if(argc > 2)
    die("Insira apenas um ID para o roteador!\n");

  id = toint(argv[1]);

  if(id < 0 || id >= MAX_ROT){
    printf("Intervalo de roteadores: [0, %d].\n", N_ROT - 1);
    die("ID de roteador inválido!\n");
  }

  for(int i = 0; i < MAX_ROT; i++)
    for(int j = 0; j < MAX_ROT; j++)
      dv_table[i].cost[j] = INF;

  dv_table[id].cost[id] = 0;

  while(1){
    system("clear");

    printf("           =- ROTEADOR %d -=\n", id);
    printf("----------------------------------------\n");
    printf("1 - Ver histórico de mensagens\n");
    printf("2 - Enviar mensagem\n");
    printf("3 - Criar enlace\n");
    printf("4 - Remover enlace\n");
    printf("5 - Sair\n");
    printf("----------------------------------------\n");
    printf("Opção: ");
    scanf("%d", &opt);
  }
  switch(opt){
    case 1:
      break;
    case 2:
      create_message();
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      printf("Saindo...");
      exit(0);
      break;
    default:
      printf("Opção %d inválida!\n", opt);
      break;
  }

  return 0;
}
