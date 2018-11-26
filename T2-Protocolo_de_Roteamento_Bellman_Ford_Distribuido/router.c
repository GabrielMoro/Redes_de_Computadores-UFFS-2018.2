#include "router.h"

Router router;
Table r_table[MAX_ROT];                                      // Tabela de roteamento
Neighbors n_table[MAX_ROT];
Dist_Vector dv_table[MAX_ROT];
Package message_in[QUEUE_SIZE], message_out;  // Filas do roteador

int sckt, id, dv_changed = 1, msg_flag = 0, message_control_in = 0, msg_id_control = 0;
int alive_flag[MAX_ROT];

struct sockaddr_in si_me, si_other;

pthread_t receive, send_message, send_vector, still_alive;
pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;

void die(char *s){
  perror(s);
  exit(1);
}

Package create_message(int type, int destination){
  Package msg;

  msg.source = id;
  msg.destination = destination;
  msg.type = type;

  if(type == MESSAGE){
    msg.msg_id = msg_id_control++;
    printf("Escreva a mensagem a ser enviada para %d:\n", destination);
    getchar();

    fgets(msg.content, MESSAGE_SIZE, stdin);
    return msg;
  }else if(type == DIST_VECTOR){
    msg.dv[id] = dv_table[id];
    return msg;
  }
  return msg;
}

void *receive_pkg(void *n){
  int slen = sizeof(si_other);

  while(1){
    Package pkg_in;
    if((recvfrom(sckt, &pkg_in, sizeof(pkg_in), 0, (struct sockaddr*) &si_me, &slen)) == -1){
        printf("Erro ao receber mensagem!\n");
    }
    if(pkg_in.type == MESSAGE){
      if(pkg_in.destination == id){
        printf("\nMensagem recebida do roteador %d\n", pkg_in.source);
        message_in[message_control_in] = pkg_in;
        message_control_in++;
      }else{
        message_out = pkg_in;
        printf("\nRetransmitindo de %d para %d\n", id, r_table[message_out.destination].next);
        msg_flag = 1;
      }
    }else if(pkg_in.type == DIST_VECTOR){
      alive_flag[pkg_in.source] = 1;
      for(int i = 0; i < MAX_ROT; i++){
        dv_table[pkg_in.source].cost[i] = pkg_in.dv[pkg_in.source].cost[i];
        if(dv_table[pkg_in.source].cost[i] == INF)
          dv_table[id].cost[i] == INF;
        if(dv_table[id].cost[i] > dv_table[pkg_in.source].cost[i] + dv_table[id].cost[pkg_in.source]){
          dv_table[id].cost[i] = dv_table[pkg_in.source].cost[i] + dv_table[id].cost[pkg_in.source];
          r_table[i].cost = dv_table[pkg_in.source].cost[i] + dv_table[id].cost[pkg_in.source];
          r_table[i].next = pkg_in.source;
          dv_changed = TRUE;
        }
      }
    }
  }
}

void *send_msg(void *n){
  Package msg;
  int next;

  while(1){
    if(msg_flag){
      pthread_mutex_lock(&send_mutex);

      msg = message_out;
      next = r_table[message_out.destination].next;

      si_other.sin_port = htons(n_table[next].port);

      if(inet_aton(n_table[next].ip, &si_other.sin_addr) == 0)
        die("Erro na obtenção do IP do destino (Mensagem)\n");
      else
      if(sendto(sckt, &msg, sizeof(msg), 0, (struct sockaddr*) &si_other, sizeof(si_other)) == -1)
        die("Erro ao enviar mensagem\n");

      msg_flag = 0;

      pthread_mutex_unlock(&send_mutex);
    }
  }
}

void transfer_dv(char why){
  Package msg;

  for(int n_id = 0; n_id < MAX_ROT; n_id++){
    if(n_table[n_id].port != -1){
      msg = create_message(DIST_VECTOR, n_id);
      si_other.sin_port = htons(n_table[n_id].port);

      if(inet_aton(n_table[n_id].ip, &si_other.sin_addr) == 0)
        die("Erro na obtenção do IP do destino (Vetores Distância)\n");
      else
      if(sendto(sckt, &msg, sizeof(msg), 0, (struct sockaddr*) &si_other, sizeof(si_other)) == -1)
        die("Erro ao enviar vetores distância\n");
      else
      if(why == 'C')
        printf("Roteador %d enviando vetores distância para roteador %d. Houve mudança na tabela de roteamento.\n", id, n_id);
      // else
      //   printf("Roteador %d enviando vetores distância para roteador %d. Passaram %d segundos.\n", id, n_id, SEND_TIME);
    }
  }
}

void *send_dv(void *n){
  time_t timer;

  timer = time(0);

  sleep(1);
  while(1){
    pthread_mutex_lock(&send_mutex);

    double dv_exec_time = difftime(time(0), timer);

    if(dv_changed == TRUE){
      transfer_dv('C');
      dv_changed = FALSE;
      timer = time(0);
    }else if(dv_exec_time >= SEND_TIME){
      transfer_dv('T');
      timer = time(0);
    }
    pthread_mutex_unlock(&send_mutex);
  }
}

void *check_alive(void *n){
  time_t timer;

  timer = time(0);

  while(1){
    if(difftime(time(0), timer) >= 10){
      pthread_mutex_lock(&send_mutex);
      for(int i = 0; i < MAX_ROT; i++){
        if(alive_flag[i] != 1 && n_table[i].port != -1){
          dv_table[id].cost[i] = INF;
          dv_table[i].cost[id] = INF;
          n_table[i].port = -1;
          dv_changed = TRUE;
        }
      }
      memset(alive_flag, 0, sizeof(alive_flag));
      pthread_mutex_unlock(&send_mutex);
      timer = time(0);
    }
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
    r_table[i].cost = INF;
    r_table[i].next = -1;
    for(int j = 0; j < MAX_ROT; j++)
      dv_table[i].cost[j] = INF;
  }

  dv_table[id].cost[id] = 0;
  r_table[id].cost = 0;
  r_table[id].next = id;

  memset(alive_flag, 0, sizeof(alive_flag));

  start_topology(id);

  pthread_create(&receive, NULL, receive_pkg, NULL);
  pthread_create(&send_vector, NULL, send_dv, NULL);
  pthread_create(&send_message, NULL, send_msg, NULL);
  pthread_create(&still_alive, NULL, check_alive, NULL);

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
        system("clear");
        for(int i = 0; i <= message_control_in; i++){
          if(i < message_control_in){
            printf("Mensagem #%d recebida de %d\n", message_in[i].msg_id, message_in[i].source);
            printf("- %s\n", message_in[i].content);
          }
        }
        printf("Pressione ENTER para prosseguir!");
        getchar();
        getchar();
        break;
      case 2:
        do{
          printf("\nDigite o roteador de destino: ");
          scanf("%d", &destination);
          if(destination < 0 || destination >= MAX_ROT)
            printf("Roteador informado não existe!\n");
        }while(destination < 0 || destination >= MAX_ROT);
        message_out = create_message(MESSAGE, destination);
        msg_flag = 1;
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
        printf("\nTabela de roteamento do roteador %d\n\n", id);
        for(int i = 0; i < MAX_ROT; i++)
          printf("Para %d - Próximo: %d, com custo: %d\n", i, r_table[i].next, r_table[i].cost);

          printf("\nPressione ENTER para prosseguir!");
          getchar();
          getchar();
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
