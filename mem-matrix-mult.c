/*
 *
 * mem_matrix_mult.c
 * Clare Bornstein
 * 
 *  
 *
 */


#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>

#include <matrix_formatter.h>

int main()
{

  //Write to temp file so we can check
  FILE* tempBuf;
  const char *mode = "w+";
  const char *tempBufName = "mbuf35x";
  tempBuf = fopen(tempBufName, mode);
  if(tempBuf == NULL)
    {
      fprintf(stderr, "Can't open buffer file %s!\n", tempBufName);
      exit(1);
    }

  //Check input lines
  char *line = NULL;
  size_t size;
  int m,n,x,y;
  int currMat = 0;

  //check first line - set m to number of columns in input
  if(getline(&line, &size, stdin) >=0)
    {
      m = 1;
      n = countWords(line);
      fprintf(tempBuf, "%s", line);
    }
  else
    {
      fprintf(stderr, "No input exists\n");
      exit(1);
    }

  //getline returns -1 on EOF
  while(getline(&line, &size, stdin) >= 0) {
    //Reset to second matrix if empty line
    if(!strcmp(line, "\n") || countWords(line) <= 1){
      if(currMat>=1) break;
      fprintf(tempBuf, "%s", line);
      x = m;
      y = n;
      if(getline(&line, &size, stdin) >=0) {
	n = countWords(line);
        m = 0;
        fprintf(tempBuf, "%s", line);
      }
      else{
        fprintf(stderr, "No input exists for second matrix\n");
        exit(1);
      }
      currMat++;
    }

    if(n != countWords(line)){
	fprintf(stderr, "Input is not a rectangular matrix. Error on line %d for n= %d\n",m,n);
      exit(1);
    }

    m++;
    fprintf(tempBuf, "%s", line);
  }

  //All stdin now held in ./mbuf35x
  //x by y first matrix, m by n second matrix
  //printf("x=%d,y=%d,m=%d,n=%d\n",x,y,m,n);
  //Check that y = m so that we can do the multiplcation - A rows = B columns
  if(y != m)
    {
      fprintf(stderr, "Matrices not compatible for multiplcation. %dx%d and %dx%d matrices\n",x,y,m,n);
      exit(1);
    }

  //Now read from ./mbuf35x with knowledge that we have x by y and m by n matrices.
  //Only need to check that inputs are numbers
  //Insert into two matrices

  rewind(tempBuf);
  char* token = NULL;
  char* cp = NULL;
  const char delimiters[] = " ";

  int ** mat1;
  int ** mat2;

  int l;
  mat1 = (int **)malloc(x * sizeof(int *));
  for (l=0; l<x; l++)
     mat1[l] = (int *)malloc(y * sizeof(int));

   mat2 = (int **)malloc(m * sizeof(int *));
   for (l=0; l<x; l++)
      mat2[l] = (int *)malloc(n * sizeof(int));

  int x1 = 0;
  int y1;

  while(getline(&line, &size, tempBuf) >= 0 && x1 < x) {
    y1 = 0;
    cp = strdup(line);
    token = strtok(cp, delimiters);

    while(token != NULL && y1 < y){
      mat1[x1][y1] = atoi(token);
      token = strtok(NULL, delimiters);
      y1++;
    }
    x1++;
  }

  //Clear new line
  getline(&line, &size, tempBuf);
  int m1 = 0;
  int n1;

  while(getline(&line, &size, tempBuf) >= 0 && m1 < m) {
    n1 = 0;
    cp = strdup(line);
    token = strtok(cp, delimiters);

    while(token != NULL && n1 < n){
      mat2[m1][n1] = atoi(token);
      token = strtok(NULL, delimiters);
      n1++;
    }
    m1++;
  }

  //printMatrix(x ,y, mat1);
  //printMatrix(m, n, mat2);

  //Start array multiplication, assuming valid inputs from above

  int mat1size = x*y;
  int mat2size = m*n;
  int outputmatsize = x*n; //output array is mat1 rows by mat2 cols
  //NOTE: x = output rows, n = output cols

  //printf("declaring shmid\n");
  int shmid; // shared memory id
  //printf("declaring shmad and shmad_cpy\n");
  int *shmad, *shmad_cpy; //shared memory address pointers

  //allocate shared memory
  //printf("trying shmget\n");
  if ((shmid = shmget(IPC_PRIVATE, (mat1size+mat2size+outputmatsize) * sizeof(int), 0600))<0){
    printf("shmid error\n");
    exit(0);
  }

  //printf("trying shmat\n");
  //Attach to address space
  if((shmad =(int *) shmat(shmid, 0, 0))==(void *)-1){
      printf("shmat error\n");
      shmctl(shmid, IPC_RMID, NULL); //release segment
      exit(0);
    }

  //Load shared resources
  shmad_cpy = shmad;

  int i, j; //iterators

  for (i=0; i<x; i++){
    for (j=0; j<y; j++){
      *shmad_cpy = mat1[i][j];
      shmad_cpy++;
    }
  }

  for (i=0; i<m; i++){
    for(j=0; j<n; j++){
      *shmad_cpy = mat2[i][j];
      //printf("%d \n", *shmad_cpy);
      shmad_cpy++;
    }
  }

  //populate output array by multiplying using fork()
  //NOTE: Output array is x rows by n columns
  //string conversions for execlp
  char shmid_s[32], i_s[8], j_s[8], x_s[8], y_s[8], m_s[8], n_s[8];
  //printf("x: %d n: %d\n", x, n);
  for (i=0; i<x; i++){
    for (j=0; j<n; j++){
      pid_t pid;
      if ((pid = fork())<0){
      	printf("fork error\n");
      	exit(0);
      }
      //printf("fork i: %d j: %d PID: %d successful\n", i, j, pid);
      if(pid == 0){ //within child process
	//convert to strings for execlp
	//printf("PID: %d in child", pid);
	snprintf(shmid_s, 32, "%d", shmid);
	snprintf(i_s, 8, "%d", i);
	snprintf(j_s, 8, "%d", j);
	snprintf(x_s, 8, "%d", x);
	snprintf(y_s, 8, "%d", y);
	snprintf(m_s, 8, "%d", m);
	snprintf(n_s, 8, "%d", n);
	if (execlp("./multiply", "multiply", shmid_s, i_s, j_s, x_s, y_s, m_s, n_s, (char *) NULL)<0){
	  printf("execlp error\n");
	  exit(0);
	  }
	exit(0);
      }
    }
  }

  //wait for all children to terminate before continuing
  for (i=0; i<outputmatsize; i++){
    waitpid(-1, NULL, 0);
  }

  //printf("All children terminated, writing to stdout\n");
  //write multiplied array to stdout
  shmad_cpy = shmad;
  shmad_cpy += (mat1size+mat2size); //track down to output matrix

  for (i = 0; i<x; i++){
    for (j = 0; j<n; j++){
      printf("%d ", *shmad_cpy);
      shmad_cpy++;
    }
    printf("\n");
  }
  printf("\n");
  //printf("releasing memory segment\n");
  shmctl(shmid, IPC_RMID, NULL); //release segment
  return 0;
}
