#include "router.h"

Router router[N_ROT];
Table r_table[N_ROT];

struct sockaddr_in si_me, si_other;

pthread_t thread_id;

int sckt;

void die(char *s){
  perror(s);
  exit(1);
}

void *receive(){
  //int slen = sizeof(si_other);
  //char a;

  while(1){
    //printf("Teste receive");
  }
}

void create_router(int r_ID){
  FILE *file = fopen("roteador.config", "r");

  if (!file)
    die("Não foi possível abrir o arquivo de configuração dos roteadores!");

  for (int i = 0; fscanf(file, "%d %d %s", &router[i].id, &router[i].port, router[i].ip) != EOF; i++);
  fclose(file);

  if((sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    die("Erro ao criar Socket\n");

  memset((char *) &si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(router[r_ID].port);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(sckt, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
    die("Erro ao dar bind!");
}

void create_links(int tab[N_ROT][N_ROT]){
  int x, y, peso;
  FILE *file = fopen("enlaces.config", "r");

  if (file){
    for (int i = 0; fscanf(file, "%d %d %d", &x, &y, &peso) != EOF; i++){
      tab[x][y] = peso;
      tab[y][x] = peso;
    }

    fclose(file);
  }
}

int verify(int *v, int value){
  for(int i = 0; i < N_ROT; i++)
    if(v[i] == value)
      return 1;
  return 0;
}

void removev(int *v, int value){
  for(int i = 0; i < N_ROT; i++)
    if(v[i] == value)
      v[i] = -1;
}

int findsminor(int *v, int *open){
  int aux = 0, minor = 1123456;

  for(int i = 0; i < N_ROT; i++)
    if(v[i] != 0 && v[i] < minor && verify(open, i)){
      minor = v[i];
      aux = i;
    }

  return aux;
}

void dijkstra(int tab_rot[N_ROT][N_ROT], int start){
  int open[N_ROT], dist[N_ROT], prev[N_ROT];    // Vércices ainda não visitados, distâncias e anteriores
  int aux_s = start, nrot = N_ROT;

  for(int i = 0; i < nrot; i++){
    open[i] = i;
    prev[i] = i;
    dist[i] = 112345;
  }

  dist[start] = 0;

  while(nrot >= 0){
    for(int i = 0; i < N_ROT; i++){
      if (tab_rot[aux_s][i] > 0 && verify(open, i) && (dist[i] > (tab_rot[aux_s][i] + dist[aux_s]))){
        dist[i] = tab_rot[aux_s][i] + dist[aux_s];
        prev[i] = aux_s;
      }
    }
    removev(open, aux_s);
    nrot--;
    aux_s = findsminor(dist, open);
  }
  backtracking(start, prev);
}

void backtracking(int start, int prev[N_ROT]){
  int a, x = 0, aux = 0, destination =  N_ROT - 1, path[N_ROT];

  if(destination == start)
    destination = 0;

  while(aux < N_ROT){
    a = aux;
    while(destination != start){
      destination = a;
      path[x] = destination;
      if(prev[destination] == start){
        r_table[start].path[aux] = path[x];
        break;
      }
      x++;
      a = prev[destination];
    }
    if(destination == start)
      r_table[start].path[destination] = start;

    x = 0;
    aux++;
    destination = aux;
  }
}

void pathcost(int start, int tab_rot[N_ROT][N_ROT]){
  for(int i = 0; i < N_ROT; i++){
    r_table[start].cost[i] = tab_rot[start][r_table[start].path[i]];
    if(start == i)
      r_table[start].cost[i] = 0;
  }
}

int toint(char *str){//converte pra int//função da internet
  int i, pot, ans;
  ans = 0;
  for(i = strlen(str) - 1, pot = 1; i >= 0; i--, pot *= 10)
    ans += pot * (str[i] - '0');
  return ans;
}

int main(int argc, char *argv[]){
  int tab_rot[N_ROT][N_ROT], opt, id, aux;

  if(argc < 2)
    die("Insira o ID do roteador!\n");
  else if(argc > 2)
    die("Insira apenas um ID para o roteador!\n");

  id = toint(argv[1]);
  if(id < 0 || id >= N_ROT){
    printf("Intervalo de roteadores: [0, %d].\n", N_ROT - 1);
    die("ID de roteador inválido!\n");
  }

  memset(tab_rot, -1, sizeof(int) * N_ROT * N_ROT);

  create_links(tab_rot);

  aux = 0;
  while(aux < N_ROT){
    dijkstra(tab_rot, aux);
    aux++;
  }

  for(int i = 0; i < N_ROT; i++){
    pathcost(i, tab_rot);
  }

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_addr.s_addr =  htonl(INADDR_ANY);

  create_router(id);

  pthread_create(&thread_id, NULL, receive, NULL);

  sleep(1);

  while(1){
    system("clear");

    printf("=- ROTEADOR %d -=\n", id);
    printf("----------------------------------------\n");
    printf("0 - Ver mensagens\n");
    printf("1 - Enviar mensagem\n");
    printf("2 - Sair\n");
    printf("----------------------------------------\n");
    scanf("%d\n", &opt);
    switch (opt){
      case 0:
        printf("Opção 0\n");
        break;
      case 1:
        printf("Opção 1\n");
        break;
      case 2:
        printf("Opção 2\n");
        break;
      default:
        printf("Opção inválida!\n");
        break;
    }
  }

  return 0;
}
