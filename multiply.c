#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]){

  //printf("starting multipy, collecting args\n");
  int shmid = atoi(argv[1]);
  int currow = atoi(argv[2]);
  int curcol = atoi(argv[3]);
  //mat1
  int x = atoi(argv[4]);
  int y = atoi(argv[5]);
  //mat2
  int m = atoi(argv[6]);
  int n = atoi(argv[7]);

  //printf("Current vars: shmid %d currow %d curcol %d x %d y %d m %d n %d\n", shmid, currow, curcol, x, y, m, n);

  int *shmad, *shmad_cpy;
  //printf("trying shmat\n");
  if((shmad = shmat(shmid, 0, 0))==(void *)-1){
    printf("shmat error\n");
    shmctl(shmid, IPC_RMID, NULL); // release segment
    exit(0);
  }

  int fullrow[y]; //row is x columns wide
  int fullcol[y]; //column is n rows long
  int i; // iterator

  //copy row from shared mem into fullrow
  //printf("fetching row from shmad\n");
  shmad_cpy = shmad;
  shmad_cpy += (currow*y); //track down to current row in mat1
  for (i=0; i<y; i++){
    fullrow[i] = *shmad_cpy;
    //printf("%d ",(int)*shmad_cpy);
    shmad_cpy++;
  } //printf("\n");

  //copy column from shared mem into fullcol
  //printf("fetching col from shmad\n");
  shmad_cpy = shmad; //track to start
  shmad_cpy += (curcol+(x*y)); //track to mat2 + current col
  for (i=0; i<y; i++){
    fullcol[i] = *shmad_cpy;
    //printf("%d ",(int) *shmad_cpy);
    shmad_cpy+=n; //wrap around to same col
  } //printf("\n");

  //calculate dot product
  //printf("calculating dot product\n");
  int total = 0;
  for(i=0; i<x; i++){
    total+=(fullrow[i]*fullcol[i]);
    //printf("%d \n", total);
  }

  //place in outputmat
  //printf("writing to outputmat\n");
  shmad_cpy = shmad; //track to start
  shmad_cpy+= ((x*y)+(m*n)); //track past 2 input mats
  shmad_cpy+= (curcol+(currow*n));
  *shmad_cpy = total;
  return 0;

}
