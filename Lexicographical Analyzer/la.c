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
  char buffer, *errorSym;
  int commmentFlag = 0;

  // If no filename is given, then the program exits
  if (argc > 1)
     ifp = fopen(argv[1], "r");
  else
    exit(0);

  printf("Source Program:\n");
  while ((buffer = fgetc(ifp)) != EOF)
  {
    errorSym = strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-*/()=,.<>;:{} \n\t", buffer);
    if (errorSym == NULL && commmentFlag == 0)
    {
      printf("BALENCIAGA");
      exit(0);
    }
    else
      printf("%c", buffer);

    if (buffer == '/' && fgetc(ifp) == '*')
    {
      printf("*");
      commmentFlag = 1;
    }

    if (commmentFlag == 0)
    {
      if (buffer != ' ' && buffer != '\n' && buffer != '\t')
      {
        inputSize++;
        inputCopy = realloc(inputCopy, sizeof(char) * inputSize);
        inputCopy[inputSize - 1] = (char)buffer;
      }
    }

    if (buffer == '*' && fgetc(ifp) == '/')
    {
      printf("/");
      commmentFlag = 0;
    }
  }

  printf("\n");
  for (int i = 0; i < inputSize; i++)
    printf("%c", inputCopy[i]);

  //`Scan for keywords and tokenize them (const, var, procedure, call, if, then, else, while, do, read, write, odd)
    /*~
      char *pvar = strstr(inputCopy, "var")
    ~*/
  //`Make sure variable names are 1-11 characters
    /*`
      Additionally, make sure variables don't start with a number -> throw error
    `*/
  //`Make sure numbers are greater than -99999 and less than 99999
  //`Discard whitespace

  // Close the file to prevent problems
  fclose(ifp);
}
