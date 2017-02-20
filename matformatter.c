#include <matrix_formatter.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int ** original;
static int ** transposed;

int main(){

  //Write to temp file so we can check
  FILE* tempBuf;
  const char *mode = "w+";
  const char *tempBufName = "mbuf35y";
  tempBuf = fopen(tempBufName, mode);
  if(tempBuf == NULL)
  {
    fprintf(stderr, "Can't open buffer file %s!\n", tempBufName);
    exit(1);
  }

  //--------Check input lines
  char *line = NULL;
  size_t size;
  int m,n;

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
    if(!strcmp(line, "\n")|| countWords(line) <= 1) break;

    if(n != countWords(line)){
      fprintf(stderr, "Input is not a rectangular matrix. Error on line %d for m= %d\n",n,m);
      exit(1);
    }

    m++;
    fprintf(tempBuf, "%s", line);
  }

  //---------Now read from ./mbuf35y with knowledge that we have m by n matrix.

  rewind(tempBuf);
  char* token = NULL;
  char* cp = NULL;
  const char delimiters[] = " ";

	int i;
  original = (int **)malloc(m * sizeof(int *));
	for (i=0; i<m; i++)
	   original[i] = (int *)malloc(n * sizeof(int));

  int m1 = 0;
  int n1;

  while(getline(&line, &size, tempBuf) >= 0 && m1 < m) {
    n1 = 0;
    cp = strdup(line);
    token = strtok(cp, delimiters);

    while(token != NULL && n1 < n){
      original[m1][n1] = atoi(token);
      token = strtok(NULL, delimiters);
      n1++;
    }
    m1++;
  }

  //All input held in original
  //printMatrix(m,n,original);

  //Print the transpose
  transposed = (int **)malloc(n * sizeof(int *));
  for (i=0; i<n; i++)
     transposed[i] = (int *)malloc(m * sizeof(int));

  transpose(m,n, original, transposed);
  printMatrix(n,m, transposed);

}
