#include "router.h"

#define DEBUG

Router router[N_ROT];
Table r_table[N_ROT];

void create_router(int r_ID){
  FILE *file = fopen("roteador.config", "r");
  if (file){
    for (int i = 0; fscanf(file, "%d %d %s", &router[i].id, &router[i].port, router[i].ip) != EOF; i++);

    #ifdef DEBUG
    for(int i = 0; i < N_ROT; i++)
      printf("%d-%d-%s\n", router[i].id, router[i].port, router[i].ip);
    #endif
  }
}

int main(){
  int tab_rot[N_ROT][N_ROT];

  create_router(0);

  return 0;
}
