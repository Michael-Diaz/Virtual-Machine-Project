# Virtual Machine (PM/0) Project
Project assignment for a Systems Software class (COP 3402) at the University of Central Florida; collaboration done w/ @Allen17202

## Usage
1. Compile the program file using the syntax "gcc la.c"
    - Additionally, use "gcc la.c -o <Custom Name>.out" to specify a name for the execuatble file
2. To run the program, use the syntax "./<Executable Name>.out <Input File Name>.txt"
3. The output from the program will be generated in a file called "output.txt"

Input files ***must*** follow the following format for the program to properly run:
  * Variables must be 11 characters in length at a Maximum
  * Numbers must be less than 99999
  * Comments are to be denotated block style (/\* ... \*/)
  * Declarations are to be done in the following syntax: \<variable name\> := \<value\>
  * Keywords: "const, var, procedure, call, if, then, else, while, do, read, write, odd"
  * Allowed Symbols: "'+’, ‘-‘, ‘*’, ‘/’, ‘(‘, ‘)’, ‘=’, ’,’ , ‘.’, ‘ <’, ‘>’,  ‘;’ , ’:’"
  
## Details
The Lexicographical Analyzer tokenizes an input file in the PM/0 code syntax using the following representations:

1. **nulsym**: Null Values
2. **identsym**: Variable Names
3. **numbersym**: Number Values
4. **plussym**: The '+' Charcater
5. **minussym**: The '-' Character
6. **multsym**: The '*' Character
7. **slashsym**: The '/' Character
8. **oddsym**: Checking if the Variable is Odd
9. **eqlsym**: The '=' Character
10. **neqsym**: The '<>' Character
11. **lessym**: The '<' Character
12. **leqsym**: The '<=' Character
13. **gtrsym**: The '>' Character
14. **geqsym**: The '>=' Character
15. **lparentsym**: The '(' Character
16. **rparentsym**: The ')' Character
17. **commasym**: The ',' Character
18. **semicolonsym**: The ';' Character
19. **periodsym**: The '.' Character
20. **becomessym**: The ':=' Character
21. **lbracesym**: The '{' Character
22. **rbracesym**: The '}' Character
23. **ifsym**: If Statements
24. **thensym**: Then Statements
25. **whilesym**: While Statements
26. **dosym**: Do Statements
27. **callsym**: Function Calls
28. **constsym**: Constant Initializations 
29. **varsym**: Variable Initializations
30. **procsym**: Function Declarations 
31. **writesym**: Writes to an auxilary file
32. **readsym**: Reads from an auxilary file
33. **elsesym**: Else Statements
