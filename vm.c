// Michael Diaz
// Gregory Allen
// COP 3402, Summer 2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  int line[3]; // OP, then L, then M
} instruction;

void main(int argc, char **argv)
{
  instruction *input;
  input = (int)malloc(0);
  int inputSize = 0;
  
  //int *code, *stack;
  //int programCounter = 0, basePointer = 1, stackPointer = 0
  //bool pcOverride = false

  //get the name of the input file
  FILE *ifp = fopen(argv[1], "r");
  char buffer[1023];
  char *word;
  int i = 0;
  
  //loop through each line; making it a struct, add struct to the dynamic array
  while (fscanf(ifp, "%s", buffer) != EOF)
  {
    word = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(word, buffer);
    
    if (i % 3 == 0)
    {
      inputSize++;
      input = (int)realloc(input, inputSize);
    }
    
    printf("%s", word);
    input[inputSize - 1].line[i % 3] = atoi(word)
    i++;

    free(word);
  }
  
  //while input[programCounter].op != return
    /*
    pcOverride = false
    //switch statement for the input[programCounter].op
      /*
      //go through all the possible operations, do whatever is needed
      */
}
