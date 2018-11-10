#include "router.h"

Router router;
Table r_table[MAX_ROT];

struct sockaddr_in si_me, si_other;

pthread_t receive_thread;

void die(char *s){
  perror(s);
  exit(1);
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

  while(1){
    system("clear");

    printf("           =- ROTEADOR %d -=\n", id);
    printf("----------------------------------------\n");
    printf("1 - Ver histórico de mensagens\n");
    printf("2 - Enviar mensagem\n");
    printf("3 - Criar/Remover enlace\n");
    printf("0 - Sair\n");
    printf("----------------------------------------\n");
    scanf("Opção: %d", &opt);
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
      system("clear");
      printf("Saindo...");
      exit(0);
      break;
    default:
      printf("Opção %d inválida!\n", opt);
      break;
  }

  return 0;
}
