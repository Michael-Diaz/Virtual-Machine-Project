// Michael Diaz
// Gregory Allen
// COP 3402, Summer 2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum { nulsym = 1, identsym, numbersym, plussym, minussym,
multsym, slashsym, oddsym,  eqlsym, neqsym, lessym,
leqsym, gtrsym, geqsym, lparentsym, rparentsym, commasym,
semicolonsym, periodsym, becomessym, lbracesym, rbracesym,
ifsym, thensym, whilesym, dosym, callsym, constsym,
varsym, procsym, writesym, readsym, elsesym
} token_type;

typedef struct
{
int kind; 		  // const = 1, var = 2, proc = 3.
char name[10];	// name up to 11 chars
int val; 		    // number (ASCII value)
int level; 		  // L  level
int adr; 		    // M  address
char kindName[11];
token_type tokens;
int errorCode;
} namerecord_t;

void main(int argc, char **argv)
{
  int i;
  int j = 0;
  char *inputCopy = malloc(sizeof(char));
  int inputSize = 0;
  FILE *ifp;
  FILE *ofp;
  int lexTokensIndex = 0;
  char buffer, *errorSym;
  int commmentFlag = 0;
  char* reservedWords[] = {"const", "var", "procedure", "call", "if", "then", "else", "while", "do", "read", "write", "odd"};
  char specialSymbols[] = {'+','-','*','/','(',')','=',',','.','<','>',';',':','{','}'};
  namerecord_t lexTokens[2000];
  int lookForward = 0;
  char bufferLookAhead;
  int errorCode = 0;
  int reservedWordFlag = -1;

  // Initialize Lex Tokens
  for(i = 0; i < 2000; i++)
  {
    lexTokens[i].tokens = 0;
  }

  // If no filename is given, then the program exits
  if (argc > 1)
     ifp = fopen(argv[1], "r");
  else
    exit(0);
  ofp = fopen("output.txt", "w");


  // Reads from file and place into array
  fprintf(ofp,"Source Program:\n");
  while ((buffer = fgetc(ifp)) != EOF)
  {
    errorSym = strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-*/()=,.<>;:{} \n\t", buffer);
    if (errorSym == NULL && commmentFlag == 0)
    {
      fprintf(ofp, "\n\nError #4 Unrecognized Symbol: %c\n", buffer);
      exit(0);
    }
    else
      fprintf(ofp, "%c", buffer);

    // Handles Comments
    if(buffer == '/')
    {
      bufferLookAhead = fgetc(ifp);
      fprintf(ofp, "%c", bufferLookAhead);
      if(bufferLookAhead == '*')
      {
        commmentFlag = 1;
      }
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

    if(buffer == '*')
    {
      bufferLookAhead = fgetc(ifp);
      fprintf(ofp, "%c", bufferLookAhead);
      if(bufferLookAhead == '/')
      {
        commmentFlag = 0;
      }
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

  inputSize++;
  inputCopy = realloc(inputCopy, sizeof(char) * inputSize);
  inputCopy[inputSize - 1] = '\0';

  fprintf(ofp, "\n\n");
  fprintf(ofp, "Lexeme Table:\n");
  fprintf(ofp, "Lexeme\t\tToken Type\n");

  // Transverses inputCopy array to get tokens
  while(inputCopy[j] != '\0')
  {
    // Character Check
    if(isalpha(inputCopy[j]))
    {
      int stringIndex = 0;
      char lexString[11];
      memset(lexString, 0, sizeof(lexString));
      lookForward = 1;
      lexString[stringIndex] = inputCopy[j];

      // check for variable names
      while (isalpha(inputCopy[++j]) || isdigit(inputCopy[j]))
      {
        if(stringIndex > 9)
        {
          lookForward = 0;
          errorCode = 1;
          break;
        }
        stringIndex++;
        lexString[stringIndex] = inputCopy[j];
      }

      // Reserved Word Check
      for(i = 0; i < 12; i++)
      {
        if(strcmp(lexString, reservedWords[i]) == 0)
        {
          reservedWordFlag = i;
          break;
        }
        else
          reservedWordFlag = 12;
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
        case 12: // identifier
          lexTokens[lexTokensIndex].tokens = identsym;
          strcpy(lexTokens[lexTokensIndex].name, lexString);
          lexTokens[lexTokensIndex].errorCode = errorCode;
          break;
      }
      lexTokensIndex++;
    }
    else if(isdigit(inputCopy[j]))
    {
      int temp;
      int numbersPlace = 1;
      int actualNum = inputCopy[j] - '0';
      lookForward = 1;

      // Get numbers and check numbers place
      while(isdigit(inputCopy[++j]))
      {

        if(numbersPlace > 4)
        {
          errorCode = 3;
          lookForward = 0;
          break;
        }
        numbersPlace++;
        temp = inputCopy[j] - '0';
        actualNum = 10 * actualNum + temp;
      }

      // Invalid variable name check
      if(isalpha(inputCopy[j]))
      {
        fprintf(ofp, "\nError #2 Variable name can not start with numbers\n");
        errorCode = 2;
        exit(0);
        break;;
      }

      lexTokens[lexTokensIndex].tokens = numbersym;
      lexTokens[lexTokensIndex].val = actualNum;
      lexTokens[lexTokensIndex].errorCode = errorCode;
      lexTokensIndex++;
    }
    else if(inputCopy[j] == '\n' ||inputCopy[j] == '\t' ||inputCopy[j] == ' ' )
    {
      j++;
      continue;
    }
    else
    {
      lookForward = 0;
      int specCharIndex = -1;

      // Special Symbol check
      for(i = 0; i < 15; i++)
      {
        if(inputCopy[j] == specialSymbols[i])
        {
          specCharIndex = i;
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
          j++;
          if(inputCopy[j] == '>')
          {
            lexTokens[lexTokensIndex].tokens = neqsym;
            strcpy(lexTokens[lexTokensIndex].kindName, "<>");
          }
          else if(inputCopy[j] == '=')
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
          j++;
          if(inputCopy[j] == '=')
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
          if(inputCopy[++j] == '=')
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
        case 13: // {
          lexTokens[lexTokensIndex].tokens = lbracesym;
          strcpy(lexTokens[lexTokensIndex].kindName, "{");
          lexTokensIndex++;
          break;
        case 14: // }
          lexTokens[lexTokensIndex].tokens = rbracesym;
          strcpy(lexTokens[lexTokensIndex].kindName, "}");
          lexTokensIndex++;
          break;
        default:
          break;
      }
    }
    if(lookForward == 0)
    {
      inputCopy[++j];
    }
  }

  // Error checks
  for(i = 0; i < lexTokensIndex; i++)
  {
    if(lexTokens[i].tokens == 2)
    {
      if(lexTokens[i].errorCode == 1)
      {
        fprintf(ofp, "\nError #1 Variable names must be less than 11 characters!\n");
        exit(0);
      }
      fprintf(ofp, "%s\t\t%d\n", lexTokens[i].name, lexTokens[i].tokens);
    }
    else if(lexTokens[i].tokens == 3)
    {
      if(lexTokens[i].errorCode == 3)
      {
        fprintf(ofp, "\nError #3 Number can only be a max of 5 digits!\n");
        exit(0);
      }
      fprintf(ofp, "%d\t\t%d\n", lexTokens[i].val, lexTokens[i].tokens);
    }
    else
    {
      fprintf(ofp, "%s\t\t%d\n", lexTokens[i].kindName, lexTokens[i].tokens);
    }
  }

  // Print to file
  fprintf(ofp, "\n");
  fprintf(ofp, "Lexeme List\n");
  fprintf(ofp, "%d ", lexTokens[0].tokens);
  if(lexTokens[0].tokens == 2)
    fprintf(ofp, "%s ", lexTokens[0].name);
  else if(lexTokens[0].tokens == 3)
    fprintf(ofp, "%d ", lexTokens[0].val);


  for(i = 1; i < lexTokensIndex; i++)
  {
    fprintf(ofp, "%d ", lexTokens[i].tokens);
    if(lexTokens[i].tokens == 2)
      fprintf(ofp, "%s ", lexTokens[i].name);
    else if(lexTokens[i].tokens == 3)
      fprintf(ofp, "%d ", lexTokens[i].val);
  }
  fprintf(ofp,"\n");

  // Close the file to prevent problems
  fclose(ifp);
  fclose(ofp);
}
