#ifndef MATRIX_FORMATTER_H
#define MATRIX_FORMATTER_H

int countWords(char* line);
void printMatrix(int rows, int cols, int** matrix);
void transpose(int rows, int cols, int** original, int** destination);

#endif
