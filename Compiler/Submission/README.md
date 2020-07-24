# Compiler (PL/0) Project
Project assignment for a Systems Software class (COP 3402) at the University of Central Florida; collaboration done w/ @Allen17202

## Usage
1. Compile the program file using the syntax "gcc h4compiler.c"
    - Alternatively, use "gcc h4compiler.c -o \<Custom Name\>.\<Extension Type\>" to specify a name for the execuatble file
2. To run the program, use the syntax "./\<Executable Name\>.\<Extension Type\>", followed by "\<Input File Name\>.txt" (If no custom name is used, default Executable is named "a" and Extension type is "out")
    - If extra console output is desired, add the following flags after "\<Input File Name\>.txt"
        - "-l" will display the Lexeme Table and Lexeme List
        - "-a" will display the Assembly Code
        - "-v" will display the Stack Manipulation
3. The output from the program will be generated in a file called "output.txt" (This will contain all flag output, regardless of which flags are active)

Input files ***must*** follow the following format for the program to properly run:
  * Variables must be 11 characters in length at a Maximum
  * Variables must begin with a letter
  * Numbers must be less than 99999
  * Comments are to be denotated block style (/\* ... \*/)
  * Declarations are to be done in the following syntax: \<variable name\> := \<value\>
  * Keywords: "const, var, procedure, call, if, then, else, while, do, read, write, odd, begin, end"
  * Allowed Symbols: "'+’, ‘-‘, ‘*’, ‘/’, ‘(‘, ‘)’, ‘=’, ’,’ , ‘.’, ‘ <’, ‘>’, ‘;’ , ’:’"
  
## Details
The Driver compiles and runs an input file in the PL/0 code syntax in the following manner:

1. The **code** is *broken* down by a lexicographical analyzer into its constituent **tokens** and are *mapped* to a symbolic reference table
2. The **tokens** are then run through a parser, which *checks* to see if the code follows the proper syntax rules that dictate the PL/0
3. The assembler takes groupings of **tokens** and *creates* a low level **assembly** language that the virtual machine can read
4. The virtual machine then *reads* in the **assembly code** and *manipulates* a **stack** as needed
