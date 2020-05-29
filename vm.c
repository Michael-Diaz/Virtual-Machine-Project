// Michael Diaz
// Gregory Allen
// COP 3402, Summer 2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Instruction
{
  int line[3]; // OP, then L, then M
} Instruction;

void main(int argc, char **argv)
{
  Instruction *input = malloc(sizeof(Instruction));
  int inputSize = 0;

  //int *code, *stack;
  int programCounter = 0, basePointer = 1, stackPointer = 0;

  //get the name of the input file
  FILE *ifp;
  char buffer[1023];
  char *word;
  int i = 0, j, k;

  if (argc > 1)
     ifp = fopen(argv[1], "r");
  else
    exit(0);

  //loop through each line; making it a struct, add struct to the dynamic array
  while (fscanf(ifp, "%s", buffer) != EOF)
  {
    word = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(word, buffer);

    // With every line, add an instruction to the array
    if (i % 3 == 0)
    {
      printf("\n"); // REMOVE LINE WHEN COMPLETE
      inputSize++;
      input = realloc(input, sizeof(Instruction) * inputSize);
    }

    printf("%s", word); // REMOVE LINE WHEN COMPLETE
    input[inputSize - 1].line[i % 3] = atoi(word);
    i++;

    free(word);
  }

  // Print the input file
  printf("\n");
  for (j = 0; j < inputSize; j++)
  {
    printf("%d", j);
    for (k = 0; k < 3; k++)
    {
      printf(" %d", input[j].line[k]);
    }
    printf("\n");
  }

  //while input[programCounter].op != return

    //switch statement for the input[programCounter].op
      /*
      //go through all the possible operations, do whatever is needed
      */

  free(input);
}
