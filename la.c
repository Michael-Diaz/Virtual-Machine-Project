// Michael Diaz
// Gregory Allen
// COP 3402, Summer 2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define  NORW 15       /* number of reserved words */
#define  IMAX 32767       /* maximum integer value */
#define  CMAX 11       /* maximum number of chars for idents */
#define  NESTMAX 5         /* maximum depth of block nesting */
#define  STRMAX 256         /* maximum length of strings */

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
} namerecord_t;

void main(int argc, char **argv)
{
  int i;
  char *inputCopy = malloc(sizeof(char));
  int inputSize = 0;
  FILE *ifp;
  int lexTokensIndex = 0;
  char buffer, *errorSym;
  int commmentFlag = 0;
  char* reservedWords[] = {"const", "var", "procedure", "call", "if", "then", "else", "while", "do", "read", "write", "odd"};
  char specialSymbols[] = {'+','-','*','/','(',')','=',',','.','<','>',';',':','{','}'};
  namerecord_t lexTokens[1000];
  int lookForward = 0;

  for(i = 0; i < 1000; i++)
  {
    lexTokens[i].tokens = 0;
  }

  // If no filename is given, then the program exits
  if (argc > 1)
     ifp = fopen(argv[1], "r");
  else
    exit(0);

  printf("Source Program:\n");
  while ((buffer = fgetc(ifp)) != EOF)
  {
    errorSym = strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-*/()=,.<>;:{} \n\t", buffer);
    if (errorSym == NULL && commmentFlag == 0)
    {
      printf("BALENCIAGA");
      exit(0);
    }
    else
      printf("%c", buffer);

    if (buffer == '/' && fgetc(ifp) == '*')
    {
      printf("*");
      commmentFlag = 1;
    }

    if (commmentFlag == 0)
    {
        inputSize++;
        inputCopy = realloc(inputCopy, sizeof(char) * inputSize);
        inputCopy[inputSize - 1] = (char)buffer;
    }

    if (buffer == '*' && fgetc(ifp) == '/')
    {
      printf("/");
      commmentFlag = 0;
    }
  }

  printf("\n");


  int j = 0;
  int reservedWordFlag = -1;
  printf("Lexeme Table:\n");
  printf("Lexeme\t\tToken Type\n");

  while(inputCopy[j] != '\0')
  {
    if(isalpha(inputCopy[j]))
    {
      int stringIndex = 0;
      char lexString[11];
      memset(lexString, 0, sizeof(lexString));
      lookForward = 1;
      lexString[stringIndex] = inputCopy[j];

      while (isalpha(inputCopy[++j]) || isdigit(inputCopy[j]))
      {
        if(stringIndex > 11)
        {
          // while (isalpha(inputCopy[++j]) || isdigit(inputCopy[j]))
          // {}
          lookForward = 0;
          printf("Name is too long\n");
          break;
        }
        stringIndex++;
        lexString[stringIndex] = inputCopy[j];
      }

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
        case 0:
          lexTokens[lexTokensIndex].tokens = constsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 1:
          lexTokens[lexTokensIndex].tokens = varsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 2:
          lexTokens[lexTokensIndex].tokens = procsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 3:
          lexTokens[lexTokensIndex].tokens = callsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 4:
          lexTokens[lexTokensIndex].tokens = ifsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 5:
          lexTokens[lexTokensIndex].tokens = thensym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 6:
          lexTokens[lexTokensIndex].tokens = elsesym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 7:
          lexTokens[lexTokensIndex].tokens = whilesym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 8:
          lexTokens[lexTokensIndex].tokens = dosym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 9:
          lexTokens[lexTokensIndex].tokens = readsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 10:
          lexTokens[lexTokensIndex].tokens = writesym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 11:
          lexTokens[lexTokensIndex].tokens = oddsym;
          strcpy(lexTokens[lexTokensIndex].kindName, lexString);
          break;
        case 12:
          lexTokens[lexTokensIndex].tokens = identsym;
          strcpy(lexTokens[lexTokensIndex].name, lexString);
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

      while(isdigit(inputCopy[++j]))
      {

        if(numbersPlace > 4)
        {
          printf("Error Number is toooooooo long!!");
          // while(isdigit(inputCopy[++j]))
          // {}
          lookForward = 0;
          break;
        }
        numbersPlace++;
        temp = inputCopy[j] - '0';
        actualNum = 10 * actualNum + temp;
      }

      if(isalpha(inputCopy[j]))
      {
        printf("Error variable name can not start with numbers");
        // while (isalpha(inputCopy[++j]) || isdigit(inputCopy[j]))
        // {}
        break;;
      }

      lexTokens[lexTokensIndex].tokens = numbersym;
      lexTokens[lexTokensIndex].val = actualNum;
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
      for(i = 0; i < 15; i++)
      {
        if(inputCopy[j] == specialSymbols[i])
        {
          specCharIndex = i;
        }
      }

      switch(specCharIndex)
      {
        case 0:
          lexTokens[lexTokensIndex].tokens = plussym;
          strcpy(lexTokens[lexTokensIndex].kindName, "+");
          lexTokensIndex++;
          break;
        case 1:
          lexTokens[lexTokensIndex].tokens = minussym;
          strcpy(lexTokens[lexTokensIndex].kindName, "-");
          lexTokensIndex++;
          break;
        case 2:
          lexTokens[lexTokensIndex].tokens = multsym;
          strcpy(lexTokens[lexTokensIndex].kindName, "*");
          lexTokensIndex++;
          break;
        case 3:
          lexTokens[lexTokensIndex].tokens = slashsym;
          strcpy(lexTokens[lexTokensIndex].kindName, "/");
          lexTokensIndex++;
          break;
        case 4:
          lexTokens[lexTokensIndex].tokens = lparentsym;
          strcpy(lexTokens[lexTokensIndex].kindName, "(");
          lexTokensIndex++;
          break;
        case 5:
          lexTokens[lexTokensIndex].tokens = rparentsym;
          strcpy(lexTokens[lexTokensIndex].kindName, ")");
          lexTokensIndex++;
          break;
        case 6:
          lexTokens[lexTokensIndex].tokens = eqlsym;
          strcpy(lexTokens[lexTokensIndex].kindName, "=");
          lexTokensIndex++;
          break;
        case 7:
          lexTokens[lexTokensIndex].tokens = commasym;
          strcpy(lexTokens[lexTokensIndex].kindName, ",");
          lexTokensIndex++;
          break;
        case 8:
          lexTokens[lexTokensIndex].tokens = periodsym;
          strcpy(lexTokens[lexTokensIndex].kindName, ".");
          lexTokensIndex++;
          break;
        case 9:
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
        case 10:
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
        case 11:
          lexTokens[lexTokensIndex].tokens = semicolonsym;
          strcpy(lexTokens[lexTokensIndex].kindName, ";");
          lexTokensIndex++;
          //printf("I made it");
          break;
        case 12:
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
        case 13:
          lexTokens[lexTokensIndex].tokens = lbracesym;
          strcpy(lexTokens[lexTokensIndex].kindName, "{");
          lexTokensIndex++;
          break;
        case 14:
          lexTokens[lexTokensIndex].tokens = rbracesym;
          strcpy(lexTokens[lexTokensIndex].kindName, "}");
          lexTokensIndex++;
          break;
        default:
          //printf("Invalid symbols\n");
          break;
      }
    }
    if(lookForward == 0)
    {
      inputCopy[++j];
    }
  }

  for(i = 0; i < lexTokensIndex; i++)
  {
  if(lexTokens[i].tokens == 2)
    printf("%s\t\t%d\n", lexTokens[i].name, lexTokens[i].tokens);
  else if(lexTokens[i].tokens == 3)
    printf("%d\t\t%d\n", lexTokens[i].val, lexTokens[i].tokens);
  else
    printf("%s\t\t%d\n", lexTokens[i].kindName, lexTokens[i].tokens);
  }
  // if(lexTokens[0].tokens == 2)
  //   printf("%s ", lexTokens[0].name);
  // else if(lexTokens[0].tokens == 3)
  //   printf("%d ", lexTokens[0].val);

  printf("\n");
  printf("Lexeme List\n");
  printf("%d ", lexTokens[0].tokens);
  if(lexTokens[0].tokens == 2)
    printf("%s ", lexTokens[0].name);
  else if(lexTokens[0].tokens == 3)
    printf("%d ", lexTokens[0].val);


  for(i = 1; i < lexTokensIndex; i++)
  {
    printf("%d ", lexTokens[i].tokens);
    if(lexTokens[i].tokens == 2)
      printf("%s ", lexTokens[i].name);
    else if(lexTokens[i].tokens == 3)
      printf("%d ", lexTokens[i].val);
  }
  printf("\n");
  //`Scan for keywords and tokenize them (const, var, procedure, call, if, then, else, while, do, read, write, odd)
    /*~
      char *pvar = strstr(inputCopy, "var")
    ~*/
  //`Make sure variable names are 1-11 characters
    /*`
      Additionally, make sure variables don't start with a number -> throw error
    `*/
  //`Make sure numbers are greater than -99999 and less than 99999
  //`Discard whitespace

  // Close the file to prevent problems
  fclose(ifp);
}
