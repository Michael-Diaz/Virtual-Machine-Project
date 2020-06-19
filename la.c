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
      //if (buffer != ' ' && buffer != '\n' && buffer != '\t')
      //{
        inputSize++;
        inputCopy = realloc(inputCopy, sizeof(char) * inputSize);
        inputCopy[inputSize - 1] = (char)buffer;
      //}
    }

    if (buffer == '*' && fgetc(ifp) == '/')
    {
      printf("/");
      commmentFlag = 0;
    }
  }

  printf("\n");
  for (int i = 0; i < inputSize; i++)
    printf("%c", inputCopy[i]);

  int j = 0;
  int reservedWordFlag = -1;
  printf("Lexeme Table:\n");
  printf("Lexeme\t\tToken Type\n");

  while(inputCopy[j] != '\0')
  {
    // if(inputCopy[j] = '\t' || inputCopy[j] == '\n' || inputCopy[j] == ' ')
    // {
    //   ;
    //   lookForward = 0;
    //   continue;
    // }
    //printf("Alpha check %d|\n", isalpha(inputCopy[j]));
    if(isalpha(inputCopy[j]))
    {
      int stringIndex = 0;
      char lexString[11];
      memset(lexString, 0, sizeof(lexString));
      lookForward = 1;
      lexString[stringIndex] = inputCopy[j];
      //stringIndex++;
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

      // for(i = 0; i < 4; i++)
      // {
      // printf("|%c|", lexString[i]);
      // }
      //printf("|%s| ", lexString);

      for(i = 0; i < 12; i++)
      {
        //printf(" %s ", reservedWords[i]);
        if(strcmp(lexString, reservedWords[i]) == 0)
        {
          reservedWordFlag = i;
          break;
        }
        else
          reservedWordFlag = 12;
      }
    //  printf("Reserved Flad %d\n", reservedWordFlag);
      switch(reservedWordFlag)
      {
        case 0:
          lexTokens[lexTokensIndex].tokens = constsym;
        //  printf("contsss");
          break;
        case 1:
          lexTokens[lexTokensIndex].tokens = varsym;
          //printf("var\n");
          break;
        case 2:
          lexTokens[lexTokensIndex].tokens = procsym;
          break;
        case 3:
          lexTokens[lexTokensIndex].tokens = callsym;
          break;
        case 4:
          lexTokens[lexTokensIndex].tokens = ifsym;
          break;
        case 5:
          lexTokens[lexTokensIndex].tokens = thensym;
          break;
        case 6:
          lexTokens[lexTokensIndex].tokens = elsesym;
          break;
        case 7:
          lexTokens[lexTokensIndex].tokens = whilesym;
          break;
        case 8:
          lexTokens[lexTokensIndex].tokens = dosym;
          break;
        case 9:
          lexTokens[lexTokensIndex].tokens = readsym;
          break;
        case 10:
          lexTokens[lexTokensIndex].tokens = writesym;
          break;
        case 11:
          lexTokens[lexTokensIndex].tokens = oddsym;
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
      //printf("InputCpy: %s", inputCopy);
      while(isdigit(inputCopy[++j]))
      {
        //printf("InputCpy: %c", inputCopy[j]);
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
      //printf("Digit: %s")

      if(isalpha(inputCopy[j]))
      {
        printf("Error variable name can not start with numbers");
        // while (isalpha(inputCopy[++j]) || isdigit(inputCopy[j]))
        // {}
        continue;
      }
      //printf("Digit: %s")
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
          printf("specCharIndex: %d\n", specCharIndex);
        }
      }

      switch(specCharIndex)
      {
        case 0:
          lexTokens[lexTokensIndex].tokens = plussym;
          lexTokensIndex++;
          break;
        case 1:
          lexTokens[lexTokensIndex].tokens = minussym;
          lexTokensIndex++;
          break;
        case 2:
          lexTokens[lexTokensIndex].tokens = multsym;
          lexTokensIndex++;
          break;
        case 3:
          lexTokens[lexTokensIndex].tokens = slashsym;
          lexTokensIndex++;
          break;
        case 4:
          lexTokens[lexTokensIndex].tokens = lparentsym;
          lexTokensIndex++;
          break;
        case 5:
          lexTokens[lexTokensIndex].tokens = rparentsym;
          lexTokensIndex++;
          break;
        case 6:
          lexTokens[lexTokensIndex].tokens = eqlsym;
          lexTokensIndex++;
          break;
        case 7:
          lexTokens[lexTokensIndex].tokens = commasym;
          lexTokensIndex++;
          break;
        case 8:
          lexTokens[lexTokensIndex].tokens = periodsym;
          lexTokensIndex++;
          break;
        case 9:
          j++;
          if(inputCopy[j] == '>')
          {
            lexTokens[lexTokensIndex].tokens = neqsym;
          }
          else if(inputCopy[j] == '=')
          {
            lexTokens[lexTokensIndex].tokens = leqsym;
          }
          else
          {
            lexTokens[lexTokensIndex].tokens = lessym;
          }
          lexTokensIndex++;
          break;
        case 10:
          j++;
          if(inputCopy[j] == '=')
          {
            lexTokens[lexTokensIndex].tokens = geqsym;
          }
          else
          {
            lexTokens[lexTokensIndex].tokens = gtrsym;
          }
          lexTokensIndex++;
          break;
        case 11:
          lexTokens[lexTokensIndex].tokens = semicolonsym;
          lexTokensIndex++;
          printf("I made it");
          break;
        case 12:
          if(inputCopy[++j] == '=')
          {
            lexTokens[lexTokensIndex].tokens = becomessym;
            lexTokensIndex++;
          }
          else
          {
            printf("This character is not supposed to be here!!!");
          }
          break;
        case 13:
          lexTokens[lexTokensIndex].tokens = lbracesym;
          lexTokensIndex++;
          break;
        case 14:
          lexTokens[lexTokensIndex].tokens = rbracesym;
          lexTokensIndex++;
          break;
        default:
          printf("Invalid symbols\n");
          break;
      }
    }
    if(lookForward == 0)
    {
      inputCopy[++j];
    }
  }

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
