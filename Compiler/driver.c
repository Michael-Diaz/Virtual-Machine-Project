// Michael Diaz
// Gregory Allen
// COP 3402, Summer 2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>


typedef enum Token_Type
{
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym, slashsym, oddsym,  eqlsym, neqsym, lessym,
  leqsym, gtrsym, geqsym, lparentsym, rparentsym, commasym,
  semicolonsym, periodsym, becomessym, beginsym, endsym,
  ifsym, thensym, whilesym, dosym, callsym, constsym,
  varsym, procsym, writesym, readsym, elsesym
} Token_Type;

typedef struct Instruction
{
  int components[3]; // OP, then L, then M
  char opName[3];
} Instruction;

typedef struct Namerecord
{
  int kind; 		      // const = 1, var = 2, proc = 3.
  char name[12];
  int val; 		        // number (ASCII value)
  int level; 		      // L  level
  int adr; 		        // M  address
  char kindName[11];  // name up to 11 chars
  Token_Type tokens;
  int errorCode;
} Namerecord;

typedef struct Symbol
{
	int kind; 		  // const = 1, var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int val; 		    // number (ASCII value)
	int level; 		  // L level
	int addr; 		  // M address
	int mark;		    // to indicate unavailable or delete d
} Symbol;

static const int MAX_DATA_STACK_HEIGHT = 1000;
static const int MAX_CODE_LENGTH = 500;
static const int NON_VALUE = INT_MIN;

static char name[12];
static Namerecord *tokens;
static int numInstructions = 0, currentToken, numberToken, kind, lexListIndex = 0, diff, prevdiff = 0;


char *arrayConvert(FILE *ifp);
Namerecord *tokenize(char *inputCopy);
void printLexemes(Namerecord *lexemes);
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

void main(int argc, char **argv)
{
  int i, flags[3] = {0, 0, 0};
  char **arguments = malloc(sizeof(char *) * argc);

  for (i = 0; i < argc; i++)
  {
    arguments[i] = argv[i];
    if (strcmp(argv[i], "-l") == 0)
        flags[0] = 1;
    else if (strcmp(argv[i], "-a") == 0)
        flags[1] = 1;
    else if (strcmp(argv[i], "-v") == 0)
        flags[2] = 1;
    else if (argv[i][0] == '-')
    {
      printf("*** Error #30, Unrecognized Flag in Driver\n");
      exit(0);
    }
  }

  FILE *input = fopen(arguments[1], "r");

  char *code = arrayConvert(input);
  tokens = tokenize(code);
  if (flags[0])
    printLexemes(tokens);

  FILE* ifp;
  ifp = fopen("output.txt", "r");

  Symbol symbolTable[1000] = {0};
  Instruction instructions[100];

  program(ifp, symbolTable, instructions);

  fclose(input);
  fclose(ifp);
}

// LEXICOGRAPHICAL ANALYZER

char *arrayConvert(FILE *ifp)
{
  char *inputCopy = malloc(sizeof(char));
  int inputSize = 0, commmentFlag = 0;
  char *errorSym;

  char buffer, bufferLookAhead;

  while ((buffer = fgetc(ifp)) != EOF)
  {
    errorSym = strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-*/()=,.<>;:{} \r\n\t", buffer);
    if (errorSym == NULL && commmentFlag == 0)
    {
      printf("*** Error #26, Unrecognized Symbol in Lex.\n");
      exit(0);
    }

    // Handles the beginning of comments
    if(buffer == '/')
    {
      bufferLookAhead = fgetc(ifp);
      printf("%c", bufferLookAhead);
      if(bufferLookAhead == '*')
        commmentFlag = 1;
      else
      {
        inputSize++;
        inputCopy = realloc(inputCopy, sizeof(char) * inputSize);
        inputCopy[inputSize - 1] = (char)buffer;
        inputSize++;
        inputCopy = realloc(inputCopy, sizeof(char) * inputSize);
        inputCopy[inputSize - 1] = (char)bufferLookAhead;
      }
    }

    if (commmentFlag == 0 && buffer != '*' && buffer != '/')
    {
        inputSize++;
        inputCopy = realloc(inputCopy, sizeof(char) * inputSize);
        inputCopy[inputSize - 1] = (char)buffer;
    }

    // Handles the end of comments
    if(buffer == '*')
    {
      bufferLookAhead = fgetc(ifp);
      printf("%c", bufferLookAhead);
      if(bufferLookAhead == '/')
        commmentFlag = 0;
      else
      {
        inputSize++;
        inputCopy = realloc(inputCopy, sizeof(char) * inputSize);
        inputCopy[inputSize - 1] = (char)buffer;
        inputSize++;
        inputCopy = realloc(inputCopy, sizeof(char) * inputSize);
        inputCopy[inputSize - 1] = (char)bufferLookAhead;
      }
    }
  }

  fclose(ifp);

  inputSize++;
  inputCopy = realloc(inputCopy, sizeof(char) * inputSize);
  inputCopy[inputSize - 1] = '\0';
  return inputCopy;
}

Namerecord *tokenize(char *inputCopy)
{
  char* reservedWords[] = {"const", "var", "procedure", "call", "if", "then", "else", "while", "do", "read", "write", "odd", "begin", "end"};
  char specialSymbols[] = {'+','-','*','/','(',')','=',',','.','<','>',';',':'};
  int lookForward = 0, errorCode = 0, reservedWordFlag = -1;
  int i = 0, j = 0, lexTokensIndex = 0;

  Namerecord *lexTokens = malloc(sizeof(Namerecord) * 2000);
  for(i = 0; i < 2000; i++)
  {
    lexTokens[i].tokens = 0;
  }
  i = 0;

  while(inputCopy[i] != '\0')
  {
    // Character Check
    if(isalpha(inputCopy[i]))
    {
      int stringIndex = 0;
      char lexString[12];
      memset(lexString, 0, sizeof(lexString));
      lookForward = 1;
      lexString[stringIndex] = inputCopy[i];

      // check for variable names
      while (isalpha(inputCopy[++i]) || isdigit(inputCopy[i]))
      {
        if(stringIndex > 9)
        {
          lookForward = 0;
          errorCode = 1;
          printf("*** Error #28, Variable Name More than 11 Chars in Lex.\n");
          exit(0);
        }
        stringIndex++;
        lexString[stringIndex] = inputCopy[i];
      }

      // Reserved Word Check
      for(j = 0; j < 14; j++)
      {
        if(strcmp(lexString, reservedWords[j]) == 0)
        {
          reservedWordFlag = j;
          break;
        }
        else
          reservedWordFlag = 14;
      }

      switch(reservedWordFlag)
      {
        case 0: // const
          lexTokens[lexTokensIndex].tokens = constsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 1: // var
          lexTokens[lexTokensIndex].tokens = varsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 2: // procedure
          lexTokens[lexTokensIndex].tokens = procsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 3: // call
          lexTokens[lexTokensIndex].tokens = callsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 4: // if
          lexTokens[lexTokensIndex].tokens = ifsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 5: // then
          lexTokens[lexTokensIndex].tokens = thensym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 6: // else
          lexTokens[lexTokensIndex].tokens = elsesym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 7: // while
          lexTokens[lexTokensIndex].tokens = whilesym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 8: // do
          lexTokens[lexTokensIndex].tokens = dosym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 9: // read
          lexTokens[lexTokensIndex].tokens = readsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 10: // write
          lexTokens[lexTokensIndex].tokens = writesym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 11: // odd
          lexTokens[lexTokensIndex].tokens = oddsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 12: // begin
          lexTokens[lexTokensIndex].tokens = beginsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 13: // end
          lexTokens[lexTokensIndex].tokens = endsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 14: // identifier
          lexTokens[lexTokensIndex].tokens = identsym;
          strcpy(lexTokens[lexTokensIndex].name, lexString);
          lexTokens[lexTokensIndex].errorCode = errorCode;
          break;
      }
      lexTokensIndex++;
    }
    else if(isdigit(inputCopy[i]))
    {
      int temp;
      int numbersPlace = 1;
      int actualNum = inputCopy[i] - '0';
      lookForward = 1;

      // Get numbers and check numbers place
      while(isdigit(inputCopy[++i]))
      {

        if(numbersPlace > 4)
        {
          errorCode = 3;
          lookForward = 0;
          printf("*** Error #25, This Number is Too Large in Parse.\n");
          exit(0);
        }
        numbersPlace++;
        temp = inputCopy[i] - '0';
        actualNum = 10 * actualNum + temp;
      }

      // Invalid variable name check
      if(isalpha(inputCopy[i]))
      {
        printf("*** Error #27, Invalid Variable Name in Lex.\n");
        errorCode = 2;
        exit(0);
        // ~REMOVED A break; CODE CHUNK HERE
      }

      lexTokens[lexTokensIndex].tokens = numbersym;
      lexTokens[lexTokensIndex].val = actualNum;
      lexTokens[lexTokensIndex].errorCode = errorCode;
      lexTokensIndex++;
    }
    else if(inputCopy[i] == '\n' || inputCopy[i] == '\t' || inputCopy[i] == ' ')
    {
      i++;
      continue;
    }
    else
    {
      lookForward = 0;
      int specCharIndex = -1;

      // Special Symbol check
      for(j = 0; j < 13; j++)
      {
        if(inputCopy[i] == specialSymbols[j])
        {
          specCharIndex = j;
          break;
        }
      }

      switch(specCharIndex)
      {
        case 0: // +
          lexTokens[lexTokensIndex].tokens = plussym;
          strcpy(lexTokens[lexTokensIndex].kindName, "+");
          lexTokensIndex++;
          break;
        case 1: // -
          lexTokens[lexTokensIndex].tokens = minussym;
          strcpy(lexTokens[lexTokensIndex].kindName, "-");
          lexTokensIndex++;
          break;
        case 2: // *
          lexTokens[lexTokensIndex].tokens = multsym;
          strcpy(lexTokens[lexTokensIndex].kindName, "*");
          lexTokensIndex++;
          break;
        case 3: // /
          lexTokens[lexTokensIndex].tokens = slashsym;
          strcpy(lexTokens[lexTokensIndex].kindName, "/");
          lexTokensIndex++;
          break;
        case 4: // (
          lexTokens[lexTokensIndex].tokens = lparentsym;
          strcpy(lexTokens[lexTokensIndex].kindName, "(");
          lexTokensIndex++;
          break;
        case 5: // )
          lexTokens[lexTokensIndex].tokens = rparentsym;
          strcpy(lexTokens[lexTokensIndex].kindName, ")");
          lexTokensIndex++;
          break;
        case 6: // =
          lexTokens[lexTokensIndex].tokens = eqlsym;
          strcpy(lexTokens[lexTokensIndex].kindName, "=");
          lexTokensIndex++;
          break;
        case 7: // ,
          lexTokens[lexTokensIndex].tokens = commasym;
          strcpy(lexTokens[lexTokensIndex].kindName, ",");
          lexTokensIndex++;
          break;
        case 8: // .
          lexTokens[lexTokensIndex].tokens = periodsym;
          strcpy(lexTokens[lexTokensIndex].kindName, ".");
          lexTokensIndex++;
          break;
        case 9: // <> <= <
          i++;
          if(inputCopy[i] == '>')
          {
            lexTokens[lexTokensIndex].tokens = neqsym;
            strcpy(lexTokens[lexTokensIndex].kindName, "<>");
          }
          else if(inputCopy[i] == '=')
          {
            lexTokens[lexTokensIndex].tokens = leqsym;
            strcpy(lexTokens[lexTokensIndex].kindName, "<=");
          }
          else
          {
            lexTokens[lexTokensIndex].tokens = lessym;
            strcpy(lexTokens[lexTokensIndex].kindName, "<");
          }
          lexTokensIndex++;
          break;
        case 10: // >= >
          i++;
          if(inputCopy[i] == '=')
          {
            lexTokens[lexTokensIndex].tokens = geqsym;
            strcpy(lexTokens[lexTokensIndex].kindName, ">=");
          }
          else
          {
            lexTokens[lexTokensIndex].tokens = gtrsym;
            strcpy(lexTokens[lexTokensIndex].kindName, ">");
          }
          lexTokensIndex++;
          break;
        case 11: // ;
          lexTokens[lexTokensIndex].tokens = semicolonsym;
          strcpy(lexTokens[lexTokensIndex].kindName, ";");
          lexTokensIndex++;
          break;
        case 12: // :=
          if(inputCopy[++i] == '=')
          {
            lexTokens[lexTokensIndex].tokens = becomessym;
            strcpy(lexTokens[lexTokensIndex].kindName, ":=");
            lexTokensIndex++;
          }
          else
          {
            printf("This character is not supposed to be here!!!");
          }
          break;
        default:
          break;
      }
    }
    if(lookForward == 0)
    {
      inputCopy[++i];
    }
  }

  FILE *ofp = fopen("output.txt", "w");
  i = 1;

  fprintf(ofp, "%d", lexTokens[0].tokens);
  if(lexTokens[0].tokens == 2)
    fprintf(ofp, "%s", lexTokens[0].name);
  else if(lexTokens[0].tokens == 3)
    fprintf(ofp, "%d", lexTokens[0].val);
  while (lexTokens[i].tokens != 0)
  {
    fprintf(ofp, " %d", lexTokens[i].tokens);
    if(lexTokens[i].tokens == 2)
      fprintf(ofp, " %s", lexTokens[i].name);
    else if(lexTokens[i].tokens == 3)
      fprintf(ofp, " %d", lexTokens[i].val);
    i++;
  }
  fprintf(ofp, "\n");
  fclose(ofp);

  return lexTokens;
}

void printLexemes(Namerecord *lexemes)
{
  int i = 1;

  printf("%d", lexemes[0].tokens);
  if(lexemes[0].tokens == 2)
    printf("%s", lexemes[0].name);
  else if(lexemes[0].tokens == 3)
    printf("%d", lexemes[0].val);
  while (lexemes[i].tokens != 0)
  {
    printf(" %d", lexemes[i].tokens);
    if(lexemes[i].tokens == 2)
      printf(" %s", lexemes[i].name);
    else if(lexemes[i].tokens == 3)
      printf(" %d", lexemes[i].val);
      i++;
  }
  printf("\n");
}

// PARSER & ASSEMBLER

void addToSymbolTable(int kind, int* tableIndexPointer, int* dataIndexPointer, int lexLevel, Symbol* symbolTable)
{
  char* nameTemp;
  int i, strLength;
  (*tableIndexPointer)++;
  nameTemp = name;
  strLength = strlen(name);

  for(i = 0;i < strLength; i++)
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
    printf("Error 9, . Expected in Parse.\n");
    exit(1);
  }

  int i;
  for(i = 0; i < numInstructions; i++)
  {
     printf("%d %d %d\n", instructions[i].components[0], instructions[i].components[1], instructions[i].components[2]);
  }
}

void block(int tableIndex,int lexLevel, FILE* ifp, Symbol* symbolTable, Instruction* instructions)
{
  int tableIndexTemp = tableIndex;
  int dataIndex = 4;
  int numInstructionsTemp;
  symbolTable[tableIndex].addr = numInstructions;
  addToOutput(7,0,0,instructions);

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
          printf("Error 5, ; or , Missing in Parse.\n");
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
          printf("Error 5, ; or , Missing in Parse.\n");
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
        printf("Error 4, Const, Var or Procedure Must be Followed by an Identifier in Parse.\n");
        exit(1);
      }
      if(currentToken == semicolonsym)
      {
        currentToken = nextToken(ifp);
      }
      else
      {
        printf("Error 5, ; or , Missing in Parse.\n");
        exit(1);
      }
      block(lexLevel+1, tableIndex, ifp, symbolTable, instructions);

      if(currentToken == semicolonsym)
      {
        currentToken = nextToken(ifp);
      }
      else
      {
        printf("Error 5, ; or , Missing in Parse.\n");
        exit(1);
      }
    }
  } while(currentToken == constsym || currentToken == varsym || currentToken == procsym);
  instructions[symbolTable[tableIndexTemp].addr].components[2] =  numInstructions;
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
      printf("Error 11, Undeclared Identifier in Parse.\n");
      //exit(1);
    }
    else if(symbolTable[i].kind != 2)
    {
      printf("Error 12, Assignment to Constant or Procedure is Not Allowed in Parse.\n");
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
      printf("Error 13, Assignment Operator Expected in Parse.\n");
      exit(0);
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
      printf("Error 14, 'call' Must be Followed by an Identifier in Parse.\n");
    }
    else
    {
      i = indexCheck(name, tableIndexPointer, symbolTable, lexLevel);
      if(i == 0)
      {
        printf("Error 11, Undeclared Identifier in Parse.\n");
        exit(1);
      }
      else if(symbolTable[i].kind == 3)
      {
        addToOutput(5, lexLevel - symbolTable[i].level, symbolTable[i].addr, instructions);
      }
      else
      {
        printf("Error 15, Call of a Constant or Variable is Meaningless in Parse.\n");
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
      printf("Error 16, 'then' Expected in Parse.\n");
      exit(1);
    }

    numInstructionsTemp1 = numInstructions;
    addToOutput(8,0,0,instructions);
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
      printf("Error 17, ; or 'end' Expected in Parse.\n");
      exit(1);
    }
  }
  else if(currentToken == whilesym)
  {
    numInstructionsTemp1 = numInstructions;
    currentToken = nextToken(ifp);
    condition(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    numInstructionsTemp2 = numInstructions;
    addToOutput(8,0,0,instructions);
    if(currentToken == dosym)
    {
      currentToken = nextToken(ifp);
    }
    else
    {
      printf("Error 18, 'do' Expected in Parse.\n");
      exit(1);
    }
    statement(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    addToOutput(7,0, numInstructionsTemp1, instructions);
    instructions[numInstructionsTemp2].components[2] = numInstructions;
  }
  else if(currentToken == writesym)
  {
    currentToken = nextToken(ifp);
    expression(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    addToOutput(9,0,1,instructions);
  }
  else if(currentToken == readsym)
  {
    currentToken = nextToken(ifp);
    addToOutput(10,0,2,instructions);
    i = indexCheck(name, tableIndexPointer, symbolTable, lexLevel);
    if(i == 0)
    {
      printf("Error 11, Undeclared Identifier in Parse.\n");
      //exit(1);
    }
    else if(symbolTable[i].kind != 2)
    {
      printf("Error 12, Assignment to Constant or Procedure is Not Allowed in Parse.\n");
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
      printf("Error 20, Relational Operator Expected in Parse.\n");
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
          addToOutput(2,0,8,instructions);
          break;
        case 10:
          addToOutput(2,0,9,instructions);
          break;
        case 11:
          addToOutput(2,0,10,instructions);
          break;
        case 12:
          addToOutput(2,0,11,instructions);
          break;
        case 13:
          addToOutput(2,0,12,instructions);
          break;
        case 14:
          addToOutput(2,0,13,instructions);
          break;
      }
    }
  }
}

void expression(int lexLevel, int *tableIndexPointer, FILE* ifp, Instruction* instructions, Symbol* symbolTable)
{
  int addition;

  if(currentToken == plussym || currentToken == minussym)
  {
    addition = currentToken;
    currentToken = nextToken(ifp);
    term(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    if(addition == minussym)
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
    addition = currentToken;
    currentToken = nextToken(ifp);
    term(lexLevel, tableIndexPointer, ifp, instructions, symbolTable);
    if(addition == plussym)
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
  int mult;
  factor(lexLevel, tableIndexPointer, symbolTable, ifp, instructions);
  while(currentToken == multsym || currentToken == slashsym)
  {
    mult = currentToken;
    currentToken = nextToken(ifp);
    factor(lexLevel, tableIndexPointer, symbolTable, ifp, instructions);
    if(mult == multsym)
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
        printf("Error 11, Undeclared Identifier in Parse.\n");
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
          printf("Error 22, ) Missing in Parse.\n");
          exit(1);
        }
      }
      currentToken = nextToken(ifp);
    }
    else if(currentToken == numbersym)
    {
      if(numberToken > 99999)
      {
        printf("Error 25, Number is Greater than 99999 in Parse.\n");
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
        printf("Error 22, ) Missing in Parse.\n");
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
    printf("Error 4, Const, Var or Procedure Must be Followed by an Identifier in Parse.\n");
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
        printf("Error 1, = Instead of := in Parse.\n");
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

// VIRTUAL MACHINE

/*
void printAssembly(Instruction* instructions)
{
  int i;
  printf("OpCode  LexLvl  MVal\n");
  for(i = 0; i < numInstructions; i++)
  {
    printf("%-3d%-3d%d\n", instructions[i].components[0], instructions[i].components[1], instructions[i].components[2]);
  }
}

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

    // With every line, add an Instruction slot to the array
    if (i % 3 == 0)
    {
      inputSize++;
      input = realloc(input, sizeof(Instruction) * inputSize);

      // Assign the opcode abbreviation to the Instruction for printings
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

    // Fill the Instruction's "component" arrays with the appropriate data
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
*/
