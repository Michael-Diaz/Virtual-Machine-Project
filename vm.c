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
  char opName[3];
} Instruction;

Instruction fetch(Instruction *input, int *pc);
void execute(Instruction *ir, int *pc, int *sp, int *bp);

void main(int argc, char **argv)
{
  // Necessary Constants described by assignment
  const int MAX_DATA_STACK_HEIGHT = 1000;
  const int MAX_CODE_LENGTH = 500;

  // Create an array to manipulate/access the instructions for the PM/0 and set its size to 0
  Instruction *input = malloc(sizeof(Instruction));
  int inputSize = 0;

  printf("*\n");

  //~~int *code, *stack;~~
  // Initialize the Registers
  int pcPtr, spPtr, bpPtr, *programCounter = &pcPtr, *stackPointer = &spPtr, *basePointer = &bpPtr;
  *programCounter = 0;
  *stackPointer = MAX_DATA_STACK_HEIGHT;
  *basePointer = *stackPointer - 1;
  Instruction instructionRegister;
  int haltFlag = 1;

  printf("**\n");

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

  printf("***\n");

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

      switch(atoi(word))
      {
        case 1:
          strcpy(input[inputSize - 1].opName, "LIT");
          break;
        case 2:
          strcpy(input[inputSize - 1].opName, "OPR");
          break;
        case 3:
          strcpy(input[inputSize - 1].opName, "LOD");
          break;
        case 4:
          strcpy(input[inputSize - 1].opName, "STO");
          break;
        case 5:
          strcpy(input[inputSize - 1].opName, "CAL");
          break;
        case 6:
          strcpy(input[inputSize - 1].opName, "INC");
          break;
        case 7:
          strcpy(input[inputSize - 1].opName, "JMP");
          break;
        case 8:
          strcpy(input[inputSize - 1].opName, "JPC");
          break;
        case 9:
        case 10:
        case 11:
          strcpy(input[inputSize - 1].opName, "SIO");
          break;
      }
    }

    // Fill the instruction's "component" arrays with the appropriate data
    input[inputSize - 1].components[i % 3] = atoi(word);
    i++;

    // Free the current scanned word to avoid a mem-leak
    free(word);
  }

  printf("****\n");

  // Print the input file
  printf("Line  OP    L     M\n");
  for (j = 0; j < inputSize; j++)
  {
    printf("%-6d%s   ", j, input[j].opName);
    for (k = 1; k < 3; k++)
    {
      printf("%-6d", input[j].components[k]);
    }
    printf("\n");
  }

  printf("*****\n");

  // Begin the fetch/execute cycles
  while(haltFlag)
  {
    instructionRegister = fetch(input, programCounter);
    *programCounter += 1;
  }

  free(input);
}

Instruction fetch(Instruction *input, int *pc)
{
  return input[*pc];
}

void execute(Instruction *ir, int *pc, int *sp, int *bp)
{
  //~~use a switch statement to do all the different operations using ir.compnenets[0]~~
  //~~manipulate the stack as needed, then update the registers (parameters)~~
  //~~print the stack and all the other records- I'm not really sure whether to do this before or after the execution???~~
}
