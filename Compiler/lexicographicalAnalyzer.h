#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum Token_Type
{
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym, slashsym, oddsym,  eqlsym, neqsym, lessym,
  leqsym, gtrsym, geqsym, lparentsym, rparentsym, commasym,
  semicolonsym, periodsym, becomessym, lbracesym, rbracesym,
  ifsym, thensym, whilesym, dosym, callsym, constsym,
  varsym, procsym, writesym, readsym, elsesym
} Token_Type;

typedef struct Namerecord
{
  int kind; 		  // const = 1, var = 2, proc = 3.
  char name[10];	// name up to 11 chars
  int val; 		    // number (ASCII value)
  int level; 		  // L  level
  int adr; 		    // M  address
  char kindName[11];
  Token_Type tokens;
  int errorCode;
} Namerecord;

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
      printf("\n\n*** Error #26, Unrecognized Symbol in Lex.\n", buffer);
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
      char lexString[11];
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
          break;
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
          lexTokens[lexTokensIndex].tokens = lbracesym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 13: // end
          lexTokens[lexTokensIndex].tokens = rbracesym;
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
          break;
        }
        numbersPlace++;
        temp = inputCopy[i] - '0';
        actualNum = 10 * actualNum + temp;
      }

      // Invalid variable name check
      if(isalpha(inputCopy[i]))
      {
        printf("\n*** Error #27, Invalid Variable Name in Lex.\n");
        errorCode = 2;
        exit(0);
        break; //~DELETE?
      }

      lexTokens[lexTokensIndex].tokens = numbersym;
      lexTokens[lexTokensIndex].val = actualNum;
      lexTokens[lexTokensIndex].errorCode = errorCode;
      lexTokensIndex++;
    }
    else if(inputCopy[i] == '\n' ||inputCopy[i] == '\t' ||inputCopy[i] == ' ')
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

  return lexTokens;
}
