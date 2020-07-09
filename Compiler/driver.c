// Michael Diaz
// Gregory Allen
// COP 3402, Summer 2020

#include "virtualMachine.h"
#include "lexicographicalAnalyzer.h"

void main(int argc, char **argv)
{
  int i = 0;

  FILE *input = fopen(argv[1], "r");
  char *code = arrayConvert(input);
  Namerecord *tokens = tokenize(code);

  while (tokens[i].tokens != 0)
  {
    printf("%d\t", tokens[i].tokens);
    switch (tokens[i].tokens)
    {
      case 2:
        printf("%s\n", tokens[i].name);
        break;
      case 3:
        printf("%d\n", tokens[i].val);
        break;
      default:
        printf("%s\n", tokens[i].kindName);
    }
    i++;
  }

  //Instruction *code = groupAssembly(input);
  //cycle(code);
}
