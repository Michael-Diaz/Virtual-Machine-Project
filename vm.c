// Michael Diaz
// Gregory Allen
// COP 3402, Summer 2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This struct contains the opcode, lexicographical level, and parameter field
typedef struct Instruction
{
  int components[3]; // OP, then L, then M
  char *opName;
} Instruction;

void main(int argc, char **argv)
{
  // Create an array to manipulate/access the instructions for the PM/0 and set its size to 0
  Instruction *input = malloc(sizeof(Instruction));
  int inputSize = 0;

  //~~int *code, *stack;~~
  int programCounter = 0, basePointer = 1, stackPointer = 0;

  // Set up the input file variable, along with buffer and allocation for the current word being scanned
  FILE *ifp;
  char buffer[1023];
  char *word;
  int i = 0, j, k;

  // If no filename is given, then the program exits
  if (argc > 1)
     ifp = fopen(argv[1], "r");
  else
    exit(0);

  // Read the file, adding the components to an instruction set and adding it to the array in the order recieved
  while (fscanf(ifp, "%s", buffer) != EOF)
  {
    word = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(word, buffer);

    // With every line, add an instruction slot to the array
    if (i % 3 == 0)
    {
      
      inputSize++;
      input = realloc(input, sizeof(Instruction) * inputSize);
    }

    // Fill the instruction's "component" arrays with the appropriate data
    input[inputSize - 1].components[i % 3] = atoi(word);
    i++;

    // Free the current scanned word to avoid a mem-leak
    free(word);
  }

  // Print the input file
  printf("Line  OP    L     M\n");
  for (j = 0; j < inputSize; j++)
  {
    printf("%-6d", j);
    for (k = 0; k < 3; k++)
    {
      printf("%-6d", input[j].components[k]);
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
