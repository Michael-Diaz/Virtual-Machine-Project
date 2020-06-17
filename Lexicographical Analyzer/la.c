// Michael Diaz
// Gregory Allen
// COP 3402, Summer 2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main(int argc, char **argv)
{
  char *inputCopy = malloc(sizeof(char));
  int inputSize = 0;

  FILE *ifp;
  char buffer;

  // If no filename is given, then the program exits
  if (argc > 1)
     ifp = fopen(argv[1], "r");
  else
    exit(0);

  printf("Source Program:\n");
  while ((buffer = fgetc(ifp)) != EOF)
  {
    inputSize++;
    inputCopy = realloc(inputCopy, sizeof(char) * inputSize);
    inputCopy[inputSize - 1] = (char)buffer;
    printf("%c", inputCopy[inputSize - 1]);
  }

  // Close the file to prevent problems
  fclose(ifp);
}
