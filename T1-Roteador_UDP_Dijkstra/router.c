#include "router.h"

#define DEBUG

Router router[N_ROT];
Table r_table[N_ROT];
int count_table = 0;

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
  int aux_s = start, nrot = N_ROT;   //HELP: AUX_S É O A, NROT É O CONTROLE

  //memset(dist, 11234, sizeof(int) * nrot);
  //memset(prev, -1, sizeof(int) * nrot);

  for(int i = 0; i < nrot; i++){
    open[i] = i;
    prev[i] = i;
    dist[i] = 112345;
  }

  dist[start] = 0;

  while(nrot >= 0){
    for(int i = 0; i < N_ROT; i++){
      #ifdef DEBUG
        printf("i = %d\n", i);
        printf("1. tab_rot[aux_s][i] = %d\n", tab_rot[aux_s][i]);
        printf("2. verify(open, i) = %d\n", verify(open, i));
        printf("3. dist[i] = %d > tab_rot[aux_s][i] + dist[aux_s] = %d\n", dist[i], tab_rot[aux_s][i] + dist[aux_s]);
      #endif
      if (tab_rot[aux_s][i] > 0 && verify(open, i) && (dist[i] > (tab_rot[aux_s][i] + dist[aux_s]))){
        dist[i] = tab_rot[aux_s][i] + dist[aux_s];
        prev[i] = aux_s;
        #ifdef DEGUB
          printf("4. new dist[i] = %d\n", dist[i]);
          printf("5. prev[i] = %d\n", prev[i]);
        #endif
      }
      printf("\n");
    }
    removev(open, aux_s);
    #ifdef DEGUB
      printf("removi aux_s = %d\n", aux_s);
    #endif
    nrot--;
    aux_s = findsminor(dist, open);
    #ifdef DEGUB
      printf("aux_s dps de findsminor = %d\n\n\n", aux_s);
    #endif
  }
  #ifdef DEGUB
    printf("\n\nIndo pro BT\n");
  #endif
  backtracking(start, prev);
}

void backtracking(int start, int prev[N_ROT]){
  int a, x = 0, aux = 0, destination =  N_ROT - 1, path[N_ROT];

  while(aux < N_ROT){
    a = aux;
    #ifdef DEGUB
      printf("\n1. Entrei no while numero = %d\n", aux);
      printf("2. destination %d != start %d\n", destination, start);
    #endif
    while(destination != start){
      destination = a;
      path[x] = destination;
      #ifdef DEGUB
        printf("3. destination = %d\n", destination);
        printf("4. x = %d\n", x);
        printf("5. path[x] = %d\n", path[x]);
        printf("6. prev = %d\n\n", prev[destination]);
      #endif
      x++;
      a = prev[destination];
    }
    #ifdef DEGUB
      printf("sai do while 2.0\n");
    #endif

    for(int i = x - 1, y = 0; i >= 0; i--, y++)
      r_table[count_table].path[y] = path[i];
    #ifdef DEGUB
      for(int i = x - 1, y = 0; i >= 0; i--, y++)
      printf("->%d", r_table[count_table].path[y]);
      printf("\n");
    #endif

    count_table++;
    x = 0;
    aux++;
    destination = aux;
  }
}

int main(){
  int tab_rot[N_ROT][N_ROT];

  memset(tab_rot, -1, sizeof(int) * N_ROT * N_ROT);

  create_router(0);
  printf("\n");
  create_links(tab_rot);

  int aux = 0;
  //while(aux < N_ROT){
    dijkstra(tab_rot, 0);
    //aux++;
  //}

  return 0;
}
