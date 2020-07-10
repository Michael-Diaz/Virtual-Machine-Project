// Michael Diaz
// Gregory Allen

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "virtualMachine.h"
#include "lexicographicalAnalyzer.h"



typedef struct Symbol
{
	int kind; 		  // const = 1, var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int val; 		    // number (ASCII value)
	int level; 		  // L level
	int addr; 		  // M address
	int mark;		    // to indicate unavailable or delete d
} Symbol;

char name[12];
Namerecord *tokens;
int numInstructions = 0, currentToken, numberToken, kind, lexListIndex = 0, diff, prevdiff = 0;

void program(FILE* ifp, Symbol* symbolTable, Instruction* instructions);
void addToOutput(int op, int l, int m, Instruction* instructions);
int nextToken(FILE* ifp);
void block(int tableIndex,int lexLevel, FILE* ifp, Symbol* symbolTable, Instruction* instructions);
void addToSymbolTable(int kind, int* tableIndexPointer, int* dataIndexPointer, int lexLevel, Symbol* table);
void constCheck(int lexLevel, int* tableIndexPointer, int* dataIndexPointer, FILE* ifp, Symbol* symbolTable);
void varCheck(int lexLevel, int* tableIndexPointer, int* dataIndexPointer, FILE* ifp, Symbol* symbolTable);
void statement(int lexLevel, int* tableIndexPointer, FILE* ifp, Instruction* instructions, Symbol* symbolTable);
int indexCheck(char* name, int* tableIndexPointer, Symbol* symbolTable, int lexLevel);
void condition(int lexLevel, int* tableIndexPointer, FILE* ifp, Instruction* instructions, Symbol* symbolTable);
void expression(int lexLevel, int *tableIndexPointer, FILE* ifp, Instruction* instructions, Symbol* symbolTable);
void term(int lexLevel, int* tableIndexPointer, FILE* ifp, Instruction* instructions, Symbol* symbolTable);
void factor(int lexLevel, int* tableIndexPointer, Symbol* symbolTable, FILE* ifp, Instruction* instructions);


void addToSymbolTable(int kind, int* tableIndexPointer, int* dataIndexPointer, int lexLevel, Symbol* symbolTable)
{
  char *nameTemp;
  int i, strLength;
  (*tableIndexPointer)++;
  nameTemp = name;
  strLength = strlen(name);

  for(i = 0; i < strLength; i++) //~STRCPY THIS BITCH
  {
    symbolTable[*tableIndexPointer].name[i] = *nameTemp;
    nameTemp++;
  }

  symbolTable[*tableIndexPointer].kind = kind;

  if(kind == 1)
  {
    symbolTable[*tableIndexPointer].val = numberToken;
  }
  else if(kind == 2)
  {
    symbolTable[*tableIndexPointer].level = lexLevel;
    symbolTable[*tableIndexPointer].addr = *dataIndexPointer;
    (*dataIndexPointer)++;
  }
  else
  {
    symbolTable[*tableIndexPointer].level = lexLevel;
  }
}

void addToOutput(int op, int l, int m, Instruction* instructions)
{
  instructions[numInstructions].components[0] = op;
  instructions[numInstructions].components[1] = l;
  instructions[numInstructions].components[2] = m;
  numInstructions++;
}

int nextToken(FILE* ifp)
{
  currentToken = tokens[lexListIndex].tokens;

  if(currentToken == 2)
  {
    strcpy(name, tokens[lexListIndex].name);
  }
  else if(currentToken == 3)
  {
    numberToken = tokens[lexListIndex].val;
  }
  lexListIndex++;
  return currentToken;
}

void program(FILE* ifp, Symbol* symbolTable, Instruction* instructions)
{
  currentToken = nextToken(ifp);
  block(0, 0, ifp, symbolTable, instructions);

  if(currentToken != periodsym)
  {
    printf("*** Error #9, Period Expected in Parse.\n");
    exit(1);
  }
}

void block(int tableIndex, int lexLevel, FILE* ifp, Symbol* symbolTable, Instruction* instructions)
{
  int tableIndexTemp = tableIndex;
  int dataIndex = 4;
  int numInstructionsTemp;
  symbolTable[tableIndex].addr = numInstructions;
  addToOutput(7, 0, 0, instructions);

  do
  {
    if(currentToken == constsym)
    {
      currentToken = nextToken(ifp);
      do
      {
        constCheck(lexLevel, &tableIndex, &dataIndex, ifp, symbolTable);
        while(currentToken == commasym)
        {
          currentToken = nextToken(ifp);
          constCheck(lexLevel, &tableIndex, &dataIndex, ifp, symbolTable);
        }
        if(currentToken == semicolonsym)
        {
          currentToken = nextToken(ifp);
        }
        else
        {
          printf("*** Error #5, Semicolon or Comma Missing in Parse.\n");
          exit(1);
        }
      } while(currentToken == identsym);
    }
    if(currentToken == varsym)
    {
      currentToken = nextToken(ifp);
      do
      {
        varCheck(lexLevel, &tableIndex, &dataIndex, ifp, symbolTable);
        while(currentToken == commasym)
        {
          currentToken = nextToken(ifp);
          varCheck(lexLevel, &tableIndex, &dataIndex, ifp, symbolTable);
        }
        if(currentToken == semicolonsym)
        {
          currentToken = nextToken(ifp);
        }
        else
        {
          printf("*** Error #5, Semicolon or Comma Missing in Parse.\n");
          exit(1);
        }
      } while(currentToken == identsym);
    }
    while(currentToken == procsym)
    {
      currentToken = nextToken(ifp);
      if(currentToken == identsym)
      {
        addToSymbolTable(3, &tableIndex, &dataIndex, lexLevel, symbolTable);
        currentToken = nextToken(ifp);
      }
      else
      {
        printf("*** Error #4, Const, Var and Procedure Must be Followed by Identifier in Parse.\n");
        exit(1);
      }
      if(currentToken == semicolonsym)
      {
        currentToken = nextToken(ifp);
      }
      else
      {
        printf("*** Error #5, Semicolon or Comma Missing in Parse.\n");
        exit(1);
      }
      block(tableIndex, lexLevel + 1, ifp, symbolTable, instructions);

      if(currentToken == semicolonsym)
      {
        currentToken = nextToken(ifp);
      }
      else
      {
        printf("*** Error #5, Semicolon or Comma Missing in Parse.\n");
        exit(1);
      }
    }
  } while(currentToken == constsym || currentToken == varsym || currentToken == procsym);
  instructions[symbolTable[tableIndexTemp].addr].components[2] = numInstructions;
	symbolTable[tableIndexTemp].addr = numInstructions;
	numInstructionsTemp = numInstructions;

  addToOutput(6, 0, dataIndex, instructions);
  statement(lexLevel, &tableIndex, ifp, instructions, symbolTable);
	if (lexLevel != 0)
  	addToOutput(2, 0, 0, instructions);
	else
		addToOutput(11, 0, 3, instructions);
}

void statement(int lexLevel, int* tableIndexPointer, FILE* ifp, Instruction* instructions, Symbol* symbolTable)
{
  int i, numInstructionsTemp1, numInstructionsTemp2;

  if(currentToken == identsym)
  {
    i = indexCheck(name, tableIndexPointer, symbolTable, lexLevel);
    if(i == 0)
    {
      printf("*** Error #11, Undeclared Identifier in Parse.\n");
      exit(1);
    }
    else if(symbolTable[i].kind != 2)
    {
      printf("*** Error #12, Assignment to Constant or Procedure is Not Allowed in Parse.\n");
      //i = 0;
      exit(1);
    }

    currentToken = nextToken(ifp);

    if(currentToken == becomessym)
    {
      currentToken = nextToken(ifp);
    }
    else
    {
      printf("*** Error #13, Assignment Operator Expected in Parse.\n");
      exit(1);
    }

    expression(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);

    if(i != 0)
    {
      addToOutput(4, lexLevel - symbolTable[i].level, symbolTable[i].addr, instructions);
    }
  }
  else if(currentToken == callsym)
  {
    currentToken = nextToken(ifp);
    if(currentToken != identsym)
    {
      printf("*** Error #14, Call Must be Followed by an Identifier in Parse.\n");
    }
    else
    {
      i = indexCheck(name, tableIndexPointer, symbolTable, lexLevel);
      if(i == 0)
      {
        printf("*** Error #11, Undeclared Identifier in Parse.\n");
        exit(1);
      }
      else if(symbolTable[i].kind == 3)
      {
        addToOutput(5, lexLevel - symbolTable[i].level, symbolTable[i].addr, instructions);
      }
      else
      {
        printf("*** Error #15, Call of a Constant or Variable is Meaningless in Parse.\n");
        exit(1);
      }
      currentToken = nextToken(ifp);
    }
  }
  else if(currentToken == ifsym)
  {
    currentToken = nextToken(ifp);
    condition(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    if(currentToken == thensym)
    {
      currentToken = nextToken(ifp);
    }
    else
    {
      printf("*** Error #16, Then Expected in Parse.\n");
      exit(1);
    }

    numInstructionsTemp1 = numInstructions;
    addToOutput(8, 0, 0, instructions);
    statement(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);

    if(currentToken == elsesym)
    {
      currentToken = nextToken(ifp);
      instructions[numInstructionsTemp1].components[2] = numInstructions + 1;
      numInstructionsTemp1 = numInstructions;
      addToOutput(7, 0, 0, instructions);
      statement(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    }
    instructions[numInstructionsTemp1].components[2] = numInstructions;

  }
  else if(currentToken == beginsym)
  {
    currentToken = nextToken(ifp);
    statement(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);

    while(currentToken == semicolonsym)
    {
      currentToken = nextToken(ifp);
      statement(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    }
    if(currentToken == endsym)
    {
      currentToken = nextToken(ifp);
    }
    else
    {
      printf("*** Error #17, Semicolon or End Expected in Parse.\n");
      exit(1);
    }
  }
  else if(currentToken == whilesym)
  {
    numInstructionsTemp1 = numInstructions;
    currentToken = nextToken(ifp);
    condition(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    numInstructionsTemp2 = numInstructions;
    addToOutput(8, 0, 0, instructions);
    if(currentToken == dosym)
    {
      currentToken = nextToken(ifp);
    }
    else
    {
      printf("*** Error #18, Do Expected in Parse.\n");
      exit(1);
    }
    statement(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    addToOutput(7, 0, numInstructionsTemp1, instructions);
    instructions[numInstructionsTemp2].components[2] = numInstructions;
  }
  else if(currentToken == writesym)
  {
    currentToken = nextToken(ifp);
    expression(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    addToOutput(9, 0, 1, instructions);
  }
  else if(currentToken == readsym)
  {
    currentToken = nextToken(ifp);
    addToOutput(10, 0, 2, instructions);
    i = indexCheck(name, tableIndexPointer, symbolTable, lexLevel);
    if(i == 0)
    {
      printf("*** Error #11, Undeclared Identifier in Parse.\n");
      exit(1);
    }
    else if(symbolTable[i].kind != 2)
    {
      printf("*** Error #12, Assignment to Constant or Procedure is Not Allowed in Parse.\n");
      exit(1);
    }

    if(i != 0)
    {
      addToOutput(4, lexLevel - symbolTable[i].level, symbolTable[i].addr, instructions);
    }
    currentToken = nextToken(ifp);
  }
}

void condition(int lexLevel, int* tableIndexPointer, FILE* ifp, Instruction* instructions, Symbol* symbolTable)
{
  int relation;
  if(currentToken == oddsym)
  {
    currentToken = nextToken(ifp);
    expression(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    addToOutput(2, 0, 6, instructions);
  }
  else
  {
    expression(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    if((currentToken != eqlsym) && (currentToken != neqsym) && (currentToken != lessym) && (currentToken != gtrsym) && (currentToken != geqsym))
    {
      printf("*** Error #20, Relational Operator Expected in Parse.\n");
      exit(1);
    }
    else
    {
      relation = currentToken;
      currentToken = nextToken(ifp);
      expression(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
      switch(relation)
      {
        case 9:
          addToOutput(2, 0, 8, instructions);
          break;
        case 10:
          addToOutput(2, 0, 9, instructions);
          break;
        case 11:
          addToOutput(2, 0, 10, instructions);
          break;
        case 12:
          addToOutput(2, 0, 11, instructions);
          break;
        case 13:
          addToOutput(2, 0, 12, instructions);
          break;
        case 14:
          addToOutput(2, 0, 13, instructions);
          break;
      }
    }
  }
}

void expression(int lexLevel, int *tableIndexPointer, FILE* ifp, Instruction* instructions, Symbol* symbolTable)
{
  int operator;

  if(currentToken == plussym || currentToken == minussym)
  {
    operator = currentToken;
    currentToken = nextToken(ifp);
    term(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    if(operator == minussym)
    {
      addToOutput(2, 0, 1, instructions);
    }
  }
  else
  {
    term(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
  }
  while(currentToken == plussym || currentToken == minussym)
  {
    operator = currentToken;
    currentToken = nextToken(ifp);
    term(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    if(operator == plussym)
    {
      addToOutput(2, 0, 2, instructions);
    }
    else
    {
      addToOutput(2, 0, 3, instructions);
    }
  }
}

void term(int lexLevel, int* tableIndexPointer, FILE* ifp, Instruction* instructions, Symbol* symbolTable)
{
  int operator;
  factor(lexLevel, tableIndexPointer, symbolTable, ifp, instructions);
  while(currentToken == multsym || currentToken == slashsym)
  {
    operator = currentToken;
    currentToken = nextToken(ifp);
    factor(lexLevel, tableIndexPointer, symbolTable, ifp, instructions);
    if(operator == multsym)
    {
      addToOutput(2, 0, 4, instructions);
    }
    else
    {
      addToOutput(2, 0, 5, instructions);
    }
  }
}

void factor(int lexLevel, int* tableIndexPointer, Symbol* symbolTable, FILE* ifp, Instruction* instructions)
{
  int i, lexLevelTemp, address, value;
  while((currentToken == identsym) || (currentToken == numbersym) || (currentToken == lparentsym))
  {
    if(currentToken == identsym)
    {
      i = indexCheck(name, tableIndexPointer, symbolTable, lexLevel);
      if(i == 0)
      {
        printf("*** Error #11, Undeclared Identifier in Parse.\n");
        exit(1);
      }
      else
      {
        kind = symbolTable[i].kind;
        lexLevelTemp = symbolTable[i].level;
        address = symbolTable[i].addr;
        value = symbolTable[i].val;

        if(kind == 1)
        {
          addToOutput(1, 0, value, instructions);
        }
        else if(kind == 2)
        {
          addToOutput(3, lexLevel - lexLevelTemp, address, instructions);
        }
        else
        {
          printf("*** Error #22, Right Parenthesis Missing in Parse.\n");
          exit(1);
        }
      }
      currentToken = nextToken(ifp);
    }
    else if(currentToken == numbersym)
    {
      if(numberToken > 99999)
      {
        printf("*** Error #25, This Number is Too Large in Parse.\n");
        exit(1);
        numberToken = 0;
      }
      addToOutput(1, 0, numberToken, instructions);
      currentToken = nextToken(ifp);
    }
    else if(currentToken == lparentsym)
    {
      currentToken = nextToken(ifp);
      expression(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
      if(currentToken == rparentsym)
      {
        currentToken = nextToken(ifp);
      }
      else
      {
        printf("*** Error #22, Right Parenthesis Missing in Parse.\n");
        exit(1);
      }
    }
  }
}

int indexCheck(char* name, int* tableIndexPointer, Symbol* symbolTable, int lexLevel)
{
  int i;
  i = *tableIndexPointer;

  int iTemp;
  int diffCount = 0;

  while(i != 0)
  {
    if(strcmp(symbolTable[i].name, name) == 0)
    {
      if(symbolTable[i].level <= lexLevel)
      {
        if(diffCount != 0)
        {
          prevdiff = diff;
        }

        diff = lexLevel - symbolTable[i].level;

        if(diffCount == 0)
        {
          iTemp = i;
        }

        if(diff < prevdiff)
        {
          iTemp = i;
        }
        diffCount++;
      }
    }
    i--;
  }

  return iTemp;
}

void varCheck(int lexLevel, int* tableIndexPointer, int* dataIndexPointer, FILE* ifp, Symbol* symbolTable)
{
  if(currentToken == identsym)
  {
    addToSymbolTable(2, tableIndexPointer, dataIndexPointer, lexLevel, symbolTable);
    currentToken = nextToken(ifp);
  }
  else
  {
    printf("*** Error #4, Const, Var and Procedure Must be Followed by Identifier in Parse.\n");
    exit(1);
  }
}

void constCheck(int lexLevel, int* tableIndexPointer, int* dataIndexPointer, FILE* ifp, Symbol* symbolTable)
{
  if(currentToken == identsym)
  {
    currentToken = nextToken(ifp);
    if((currentToken == eqlsym) || (currentToken == becomessym))
    {
      if(currentToken == becomessym)
      {
        printf("*** Error #1, = Instead of := in Parse.\n");
        exit(1);
      }
      currentToken = nextToken(ifp);
      if(currentToken == numbersym)
      {
        addToSymbolTable(1,tableIndexPointer,dataIndexPointer, lexLevel, symbolTable);
        currentToken = nextToken(ifp);
      }
    }
  }
}

void main(int argc, int**argv)
{
	int i = 0;
  FILE *input = fopen("input.txt", "r");
  char *code = arrayConvert(input);
  tokens = tokenize(code);

	printLexemes(tokens);

 	int lexLevel = 0, dataIndex = 0, tableIndex = 0;
  FILE* ifp;
  FILE* ofp;

  ifp = fopen("output.txt", "r");
  ofp = fopen("parseOutput.txt", "w");

  Symbol symbolTable[1000] = {0};
  Instruction instructions[100];

  program(ifp, symbolTable, instructions);

  for(i = 0; i < numInstructions; i++)
  {
  	printf("%d %d %d\n", instructions[i].components[0], instructions[i].components[1], instructions[i].components[2]);
  }
  fclose(input);
  fclose(ifp);
  fclose(ofp);
}
