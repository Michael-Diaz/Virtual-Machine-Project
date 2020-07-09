#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static const int MAX_DATA_STACK_HEIGHT = 1000;
static const int MAX_CODE_LENGTH = 500;
static const int NON_VALUE = INT_MIN;

typedef struct Instruction
{
  int components[3]; // OP, then L, then M
  char opName[3];
} Instruction;

Instruction *groupAssembly(FILE *ifp)
{
  Instruction *input = malloc(sizeof(Instruction));
  int inputSize = 0;

  char buffer[1023];
  char *word;
  int i = 0;

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

  return input;
}

void printStack(int *stack, int *sp, int *ar)
{
  int i, j = 0;
  for(i = MAX_DATA_STACK_HEIGHT; i > *sp; i--)
  {
    printf("%d ", stack[i - 1]);
    if (i - 1 == ar[j])
    {
      printf("| ");
      j++;
    }
  }
  printf("\n");
}

int base(int level, int base, int *stack)
{
  int b1 = base;
  while (level > 0)
  {
    b1 = stack[b1 - 1];
    level--;
  }
  return b1;
}

Instruction fetch(Instruction *input, int *pc)
{
  return input[*pc];
}

void execute(Instruction ir, int *pc, int *sp, int *bp, int *hFlag, int *stack, int *ar, int *arNum, int *aFlag)
{
  int input;

  switch(ir.components[0])
  {
    case 1: // LIT
      *sp -= 1;
      stack[*sp] = ir.components[2];
      *pc += 1;
      break;
    case 2: // OPR
      switch(ir.components[2])
      {
        case 0: // RET
          ar[(*arNum) - 1] = NON_VALUE;
          (*arNum)--;
          ar = realloc(ar, sizeof(int) * (*arNum));
          *sp = *bp + 1;
          *pc = stack[*sp - 4];
          *bp = stack[*sp - 3];
          break;
        case 1: // NEG
          *pc += 1;
          stack[*sp] *= -1;
          break;
        case 2: // ADD
          *pc += 1;
          *sp += 1;
          stack[*sp] = stack[*sp] + stack[*sp - 1];
          break;
        case 3: // SUB
          *pc += 1;
          *sp += 1;
          stack[*sp] = stack[*sp] - stack[*sp - 1];
          break;
        case 4: // MUL
          *pc += 1;
          *sp += 1;
          stack[*sp] = stack[*sp] * stack[*sp - 1];
          break;
        case 5: // DIV
          *pc += 1;
          *sp += 1;
          stack[*sp] = stack[*sp] / stack[*sp - 1];
          break;
        case 6: // ODD
          *pc += 1;
          if(stack[*sp] % 2)
            stack[*sp] = 0;
          else
            stack[*sp] = 1;
          break;
        case 7: // MOD
          *pc += 1;
          *sp += 1;
          stack[*sp] = stack[*sp] % stack[*sp - 1];
          break;
        case 8: // EQL
          *pc += 1;
          *sp += 1;
          if (stack[*sp] == stack[*sp - 1])
            stack[*sp] = 1;
          else
            stack[*sp] = 0;
          break;
        case 9: // NEQ
          *pc += 1;
          *sp += 1;
          if (stack[*sp] != stack[*sp - 1])
            stack[*sp] = 1;
          else
            stack[*sp] = 0;
          break;
        case 10: // LSS
          *pc += 1;
          *sp += 1;
          if (stack[*sp] < stack[*sp - 1])
            stack[*sp] = 1;
          else
            stack[*sp] = 0;
          break;
        case 11: // LEQ
          *pc += 1;
          *sp += 1;
          if (stack[*sp] <= stack[*sp - 1])
            stack[*sp] = 1;
          else
            stack[*sp] = 0;
          break;
        case 12: // GTR
          *pc += 1;
          *sp += 1;
          if (stack[*sp] > stack[*sp - 1])
            stack[*sp] = 1;
          else
            stack[*sp] = 0;
          break;
        case 13: // GEQ
          *pc += 1;
          *sp += 1;
          if (stack[*sp] >= stack[*sp - 1])
            stack[*sp] = 1;
          else
            stack[*sp] = 0;
          break;
      }
      break;
    case 3: // LOD
      *sp -= 1;
      stack[*sp] = stack[base(ir.components[1], *bp, stack) - ir.components[2]];
      *pc += 1;
      break;
    case 4: // STO
      stack[base(ir.components[1], *bp, stack) - ir.components[2]] = stack[*sp];
      *sp += 1;
      *pc += 1;
      break;
    case 5: // CAL
      // Flag to add a bracket to symbolize a new activation record
      *aFlag = 1;
      *pc += 1;
      stack[*sp - 1] = 0;
      stack[*sp - 2] = base(ir.components[1], *bp, stack);
      stack[*sp - 3] = *bp;
      stack[*sp - 4] = *pc;
      *bp = *sp - 1;
      *pc = ir.components[2];
      break;
    case 6: // INC
      *sp -= ir.components[2];
      *pc += 1;
      break;
    case 7: // JMP
      *pc = ir.components[2];
      break;
    case 8: // JPC
      if(stack[*sp] == 0)
        *pc = ir.components[2];
      *sp += 1;
      break;
    case 9: // SIO 1
      printf("%d", stack[*sp]);
      *sp += 1;
      *pc += 1;
      break;
    case 10: // SIO 2
      *sp -= 1;
      scanf("Input a number: %d", &input);
      stack[*sp] = input;
      *pc += 1;
      break;
    case 11: // SIO 3
      *hFlag = 0;
      *pc += 1;
      break;
  }
}

void cycle(Instruction *input)
{
  Instruction instructionRegister;
  int *stack = malloc(sizeof(int) * MAX_DATA_STACK_HEIGHT);
  int aRecords = 0, *activationRecords = malloc(sizeof(int));
  int haltFlag = 1, aRFlag = 0;

  int pcPtr, spPtr, bpPtr, *programCounter = &pcPtr, *stackPointer = &spPtr, *basePointer = &bpPtr;
  *programCounter = 0;
  *stackPointer = MAX_DATA_STACK_HEIGHT;
  *basePointer = *stackPointer - 1;

  printf("\n                    PC    BP    SP     Stack\n");
  printf("Initial Values      0     999   1000\n");
  while(haltFlag == 1)
  {
    // Fetch cycle
    instructionRegister = fetch(input, programCounter);
    printf("%-4d%s %-4d%-8d", *programCounter, instructionRegister.opName, instructionRegister.components[1], instructionRegister.components[2]);

    // Execute Cycle
    execute(instructionRegister, programCounter, stackPointer, basePointer, &haltFlag, stack, activationRecords, &aRecords, &aRFlag);
    printf("%-6d%-6d%-6d ", *programCounter, *basePointer, *stackPointer);

    if (haltFlag == 1)
      printStack(stack, stackPointer, activationRecords);
    else
      printf("\n");

    if (aRFlag == 1)
    {
      // Keeps track of where the activation records begins to denotate them while printing the stack
      aRecords++;
      activationRecords = realloc(activationRecords, sizeof(int) * aRecords);
      activationRecords[aRecords - 1] = *stackPointer;
    }
    aRFlag = 0;
  }

  free(input);
  free(activationRecords);
}
