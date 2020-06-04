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
void execute(Instruction ir, int *pc, int *sp, int *bp, int *flag);

void main(int argc, char **argv)
{
  // Necessary Constants described by assignment
  const int MAX_DATA_STACK_HEIGHT = 1000;
  const int MAX_CODE_LENGTH = 500;

  // Create an array to manipulate/access the instructions for the PM/0 and set its size to 0
  // Also create a 2d array to store muliple stacks on different lexicographical levels
  Instruction *input = malloc(sizeof(Instruction));
  int **stack = malloc(sizeof(int *));
  int inputSize = 0, stackHeight = 0;

  // Initialize the Registers
  int pcPtr, spPtr, bpPtr, *programCounter = &pcPtr, *stackPointer = &spPtr, *basePointer = &bpPtr;
  *programCounter = 0;
  *stackPointer = 0;
  *basePointer = *stackPointer + 1;
  Instruction instructionRegister;
  int haltFlag = 1;

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

      // Assign the opcode abbreviation to the instruction for printings
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

    // Exit the scanning loop if there are more lines of code than the maximum allowed limit
    if (i == MAX_CODE_LENGTH * 3)
      break;
  }
  // Close the file to prevent problems
  fclose(ifp);

  // Print the input
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

  // Prints the stack header
  printf("\n                    PC    BP    SP    Stack\n");
  printf("Intial Values       0     999   1000\n");
  // Continues the program until it reaches the (SIO, 0, 3) command that sets the halt flag to 0 and stops the program
  while(haltFlag == 1)
  {
    // Fetch cycle
    instructionRegister = fetch(input, programCounter);
    printf("%-4d%s %-4d%-8d", *programCounter, instructionRegister.opName, instructionRegister.components[1], instructionRegister.components[2]);

    // Execute Cycle
    execute(instructionRegister, programCounter, stackPointer, basePointer, &haltFlag);
    printf("%-6d%-6d%-6d\n", *programCounter, MAX_DATA_STACK_HEIGHT - *basePointer, MAX_DATA_STACK_HEIGHT - *stackPointer);
  }

  free(input);
}

// Gets the instruction at the 'address' specified by the programCounter value
Instruction fetch(Instruction *input, int *pc)
{
  return input[*pc];
}

// Uses the opcode and other parameters to manipulate the stack
void execute(Instruction ir, int *pc, int *sp, int *bp, int *flag)
{
  switch(ir.components[0])
  {
    case 1: // LIT
      *sp += 1;
      //~append register.M onto the stack~
      *pc += 1;
      break;
    case 2: // OPR
      switch(ir.components[2])
      {
        case 0: // RET
          *sp = *bp - 1;
          *pc = 16; //~FIX THIS LINE, CONSTANT IS MEANT TO PREVENT INFINITE LOOP IN TESTCASE 1
          //~bp = stack[sp - 3]
          break;
        case 1: // NEG
          *pc += 1;
          break;
        case 2: // ADD
          *pc += 1;
          break;
        case 3: // SUB
          *pc += 1;
          break;
        case 4: // MUL
          *pc += 1;
          break;
        case 5: // DIV
          *pc += 1;
          break;
        case 6: // ODD
          *pc += 1;
          break;
        case 7: // MOD
          *pc += 1;
          break;
        case 8: // EQL
          *pc += 1;
          break;
        case 9: // NEQ
          *pc += 1;
          break;
        case 10: // LSS
          *pc += 1;
          break;
        case 11: // LEQ
          *pc += 1;
          break;
        case 12: // GTR
          *pc += 1;
          break;
        case 13: // GEQ
          *pc += 1;
          break;
      }
      break;
    case 3: // LOD
      *sp += 1;
      //~
      *pc += 1;
      break;
    case 4: // STO
      //~
      *sp -= 1;
      *pc += 1;
      break;
    case 5: // CAL
      //~
      //~
      //~
      //~
      *bp = *sp + 1;
      *pc = ir.components[2];
      break;
    case 6: // INC
      *sp += ir.components[2];
      *pc += 1;
      break;
    case 7: // JMP
      *pc = ir.components[2];
      break;
    case 8: // JPC
      //~
      *sp -= 1;
      break;
    case 9: // SIO 1
      //~
      *sp -= 1;
      *pc += 1;
      break;
    case 10: // SIO 2
      *sp += 1;
      //~
      *pc += 1;
      break;
    case 11: // SIO 3
      *flag = 0;
      *pc += 1;
      break;
  }
}
