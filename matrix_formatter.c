#include <matrix_formatter.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//returns # of "words" (numbers) in a line - assumes no newline
int countWords(char* line){
  int i;
  int count = 0;
  for(i = 0; i < strlen(line); i++){
    if(!((int) line[i] <=57 && (int)line[i] >=48)){
	 count++;
    }
  }

  i = strlen(line)-1;

  while(i >= 0 && !((int) line[i] <=57 && (int)line[i] >=48)){
	count--;
	i--;
  }

  return count+1;
}

void printMatrix(int rows, int cols, int ** matrix){
  int i,j;
  for(i=0; i < rows; i++){
      for(j=0; j < cols; j++){
        printf("%d ", matrix[i][j]);
      }
      printf("\n");
  }
}

void transpose(int rows, int cols, int** original, int** destination){
  int r,c; //iterators
  for(r = 0 ; r < rows; r++){
    for(c = 0; c < cols; c++){
      destination[c][r] = original[r][c];
    }
  }
}
