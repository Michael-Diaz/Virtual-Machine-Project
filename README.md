# Virtual Machine (PM/0) Project
Project assignment for a Systems Software class (COP 3402) at the University of Central Florida; collaboration done w/ @Allen 17202

## Usage
Input files **must** follow the following format:
  * Each line must consist of 3 non-negative whole numbers
  * The 3 numbers correspond to the operation code, lexicographical level, and a tertiary data field dependent on the operation (see "Details")

1. Compile the program file using the syntax "gcc vm.c"
    - Additionally, use "gcc vm.c -o <Custom Name>.out" to specify a name for the execuatble file
2. To run the program, use the syntax "./<Executable Name>.out <Input File Name>.txt"
  
## Details
The P-Machine is a virtual machine that manipulates stacks through 2 memory stores (the "Stack" and "Text" segments) and 4 registers (Base Pointer, Stack Pointer, Program Counter, and Instruction Register). It does this through the 11 instructions- along with 13 arithmetic/logic and 1 returning sub-operation- as follows:

1. **LIT, 0, M**: *Push constant* M *onto the stack*
2. **OPR, 0, M**: *Performs one of the operations below on the data at the top of the stack based on the value on* M
    - **M = 0**: *Return*
    - **M = 1**: *Negation*
    - **M = 2**: *Addition of the top 2 values in the stack*
    - **M = 3**: *Subtraction of the secondmost top value from the top value in the stack*
    - **M = 4**: *Multiplication of the top 2 values in the stack*
    - **M = 5**: *Division of the top value by the secondmost top value in the stack*
    - **M = 6**: *ODD???*
    - **M = 7**: *Modulus of the top value by the secondmost top value in the stack*
    - **M = 8**: *Comparison of whether the top 2 values in the stack are equal*
    - **M = 9**: *Comparison of whether the top 2 values in the stack are inequal*
    - **M = 10**: *Comparison of whether the top value in the stack is less than the secondmost top value*
    - **M = 11**: *Comparison of whether the top value in the stack is less than or equal to the secondmost top value*
    - **M = 12**: *Comparison of whether the top value in the stack is greater than the secondmost top value*
    - **M = 13**: *Comparison of whether the top value in the stack is greater than or equal to the secondmost top value*
    
3. **LOD, L, M**: *Load value to top of stack in the stack location at offset* M *from* L *lexicographical levels down*
4. **STO, L, M**: *Store value at top of stack in the stack location at offset* M *from* L *lexicographical levels down*
5. **CAL, L, M**: *Call procedure at code index* M *(Generates a new Activation Record and sets the program counter to* M
6. **INC, 0, M**: *Allocate* M *locals (Increment the stack pointer by* M *). First three are Static Link (SL), Dynamic Link (DL), and Return Address (RA)*
7. **JMP, 0, M**: *Jump to instruction #* M
8. **JPC, 0, M**: *Jump to instruction #* M *if top stack element is 0*
9. **SIO, 0, 1**: *Write the top stack element to the screen*
10. **SIO, 0, 2**: *Read in input from the user and store it on top of the stack*
11. **SIO, 0, 3**: *End program*
