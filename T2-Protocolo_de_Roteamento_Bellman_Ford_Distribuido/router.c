#include "router.h"

Router router;
Table r_table[MAX_ROT];                                      // Tabela de roteamento
Neighbors n_table[MAX_ROT];
Dist_Vector dv_table[MAX_ROT];
// Package message_in[QUEUE_SIZE], message_out[QUEUE_SIZE];  // Filas do roteador

int sckt, id;

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

void send_dist_vector(Package msg, int n_id){
  printf("n_id = %d, port = %d, ip = %s\n", n_id, n_table[n_id].port, n_table[n_id].ip);

  sleep(1);
  si_other.sin_port = htons(n_table[n_id].port);
  printf("sin_port = %d\n", si_other.sin_port);

  if(inet_aton(n_table[n_id].ip, &si_other.sin_addr) == 0)
    die("Erro na obtenção do IP do destino (Vetores Distância)\n");
  else
    if(sendto(sckt, &msg, sizeof(msg), 0, (struct sockaddr*) &si_other, sizeof(si_other)) == -1)
      die("Erro ao enviar vetores distância\n");
    else
      printf("Roteador %d enviando vetores distância para roteador %d\n", id, n_id);
}

void create_message(int type, int destination){
  Package msg;

  msg.source = id;
  msg.destination = destination;
  msg.type = type;

  if(type == MESSAGE){
    printf("Escreva a mensagem a ser enviada para %d:\n", destination);
    getchar();

    fgets(msg.content, MESSAGE_SIZE, stdin);
  }else if(type == DIST_VECTOR){
    for(int i = 0; i < MAX_ROT; i++)
      msg.dv[i] = dv_table[i];
    send_dist_vector(msg, destination);
  }
}

void start_topology(int r_id){
  FILE *links = fopen("enlaces.config", "r");
  FILE *config = fopen("roteador.config", "r");
  int x, y, w;
  int l_id, port;
  char ip[30];

  if(links && config){
    for(int i = 0; fscanf(links, "%d %d %d", &x, &y, &w) != EOF; i++){
      for(int j = 0; fscanf(config, "%d %d %s", &l_id, &port, ip) != EOF; j++){
        if(r_id == x){
          if(l_id == y){
            n_table[y].port = port;
            strcpy(n_table[y].ip, ip);
            r_table[y].next = y;
            r_table[y].cost = w;
            dv_table[r_id].cost[y] = w;
            continue;
          }
        }
        if(r_id == y){
          if(l_id == x){
            n_table[x].port = port;
            strcpy(n_table[x].ip, ip);
            r_table[x].next = x;
            r_table[x].cost = w;
            dv_table[r_id].cost[x] = w;
          }
        }
      }
      rewind(config);
    }
  }
}

void router_config(int r_id){// Configura o roteador com base no arquivo router.config
  FILE *config = fopen("roteador.config", "r");

  if(config){
    int l_id, port;
    char ip[30];
    for(int i = 0; fscanf(config, "%d %d %s", &l_id, &port, ip) != EOF; i++)
      if(l_id != r_id)
        continue;
      else{
        router.id = id;
        router.port = port;
        strcpy(router.ip, ip);

        printf("ID: %d\nPorta: %d\nIP: %s\n", router.id, router.port, router.ip);
      }
  }else
    die("Erro ao configurar o roteador\n");
  fclose(config);

  if((sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    die("Erro ao criar Socket\n");

  memset((char *) &si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(router.port);
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
  int opt, destination = -1;

  if(argc < 2)
    die("Insira o ID do roteador!\n");
  else if(argc > 2)
    die("Insira apenas um ID para o roteador!\n");

  id = toint(argv[1]);

  if(id < 0 || id >= MAX_ROT){
    die("ID de roteador inválido!\n");
  }

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_addr.s_addr =  htonl(INADDR_ANY);

  router_config(id);

  for(int i = 0; i < MAX_ROT; i++){
    n_table[i].port = -1;
    for(int j = 0; j < MAX_ROT; j++)
      dv_table[i].cost[j] = INF;
  }

  dv_table[id].cost[id] = 0;
  start_topology(id);

  // pthread_create(&thread_id, NULL, receive, NULL);

  while(1){
    system("clear");

    printf("           =- ROTEADOR %d -=\n", id);
    printf("----------------------------------------\n");
    printf("1 - Ver histórico de mensagens\n");
    printf("2 - Enviar mensagem\n");
    printf("3 - Criar enlace\n");
    printf("4 - Remover enlace\n");
    printf("5 - Mostar vetores distancia\n");
    printf("0 - Sair\n");
    printf("----------------------------------------\n");
    printf("Opção: ");
    scanf("%d", &opt);
    switch(opt){
      case 1:
        break;
      case 2:
        do{
          printf("\nDigite o roteador de destino: ");
          scanf("%d", &destination);
          if(destination < 0 || destination >= MAX_ROT)
            printf("Roteador informado não existe!\n\n");
        }while(destination < 0 || destination >= MAX_ROT);
        create_message(MESSAGE, destination);
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        for(int i = 0; i < MAX_ROT; i++){
          printf("\n");
          for(int j = 0; j < MAX_ROT; j++)
            printf("Origem: %d para destino: %d, com custo = %d\n", i, j, dv_table[i].cost[j]);
        }
        printf("\nPressione ENTER para prosseguir!");
        getchar();
        getchar();
        break;
      case 6:
        for(int i = 0; i < MAX_ROT; i++)
          if(n_table[i].port != -1)
            create_message(DIST_VECTOR, i);
        break;
      case 0:
        printf("\nSaindo...\n\n");
        exit(0);
        break;
      default:
        printf("Opção %d inválida!\n", opt);
        break;
    }
  }
  return 0;
}
