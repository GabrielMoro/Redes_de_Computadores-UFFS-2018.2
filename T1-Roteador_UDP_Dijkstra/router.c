#include "router.h"

#define DEBUG

Router router[N_ROT];
Table r_table[N_ROT];

void create_router(int r_ID){
  FILE *file = fopen("roteador.config", "r");

  if (file){
    for (int i = 0; fscanf(file, "%d %d %s", &router[i].id, &router[i].port, router[i].ip) != EOF; i++);

    fclose(file);

    #ifdef DEBUG
    for(int i = 0; i < N_ROT; i++)
      printf("%d %d %s\n", router[i].id, router[i].port, router[i].ip);
    #endif
  }
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

    #ifdef DEBUG
    for(int i = 0; i < N_ROT; i++){
      for (int j = 0; j < N_ROT; j++)
        printf("%2d ", tab[i][j]);
      printf("\n");
    }
    #endif
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
      v[i] = 0;
}

int findsminor(int *v, int *open){
  int aux = 0, minor = 1123456;

  for(int = 0; i < N_ROT; i++)
    if(v[i] != 0 && v[i] < minor && verify(open, a)){
      minor = v[i];
      aux = i;
    }

  return aux;
}

void dijkstra(int tab_rot[N_ROT][N_ROT], int start){
  int open[N_ROT], dist[N_ROT], prev[N_ROT];    // Vércices ainda não visitados, distâncias e anteriores
  int aux_s = start, nrot = N_ROT;   //HELP: AUX_S É O A, NROT É O CONTROLE

  memset(dist, 1123456, sizeof(int) * nrot);
  memset(prev, -1, sizeof(int) * nrot);

  for(int i = 0; i < nrot; i++)
    open[i] = i;

  dist[start] = 0;

  while(nrot >= 0){
    for(int i = 0; i < N_ROT; i++){
      if (tab_rot[aux_s][i] > 0 && verify(open, i) && (dist[b] > (tab_rot[aux_s][i] + dist[aux_s]))){
        dist[i] = tab_rot[aux_s][i] + dist[aux_s];
        prev[i] = aux_s;
      }
    }
    removev(open, aux_s);
    nrot--;
    aux_s = findsminor(dist, open);
  }

  //backtracking(passa inicial, e passa vetor de anteriores)
}

int main(){
  int tab_rot[N_ROT][N_ROT];

  memset(tab_rot, -1, sizeof(int) * N_ROT * N_ROT);

  create_router(0);
  printf("\n");
  create_links(tab_rot);

  int aux = 0;
  while(aux < N_ROT){
    dijkstra(tab_rot, aux);
    aux++;
  }

  return 0;
}
