## CSLang - C Stack (based) Language
(note: this project is practically useless and is only used for me to learn how to write programming languages)

CSLang is a stack based "programming" ""language"" written in C\
there are no variables, only the stack itself (and maybe a singular register in the future)\

### Installation
```bash
## to build the interpeter
git clone https://github.com/flippyshoe1/cslang.git
cd cslang
make

## to get rid of it
make clean

## NOTE: it doesnt install anything, just compiles the code
```

### Available Commands
| Command | Arguments | Description |
| ------- | --------- | ----------- |
| PUSH    | int       | Pushes an integer onto the program stack |
| POP     | nil       | Pops an integer from the stack, doesnt store it anywhere atm |
| ADD     | nil       | Pops the two leading integers in the stack, adds them and pushes the result |
| SUB     | nil       | Pops the two leading integers in the stack, subtracts them and pushes the result |
| PRINT   | string?   | prints a string to stdout, if no string is provided, prints the leading integer |
| READ    | nil       | reads an integer from stdin, pushes it to the stack
| GOTO    | label     | goes to a label |
| GOEQ    | label     | goes to a label if the 2 leading integers are equal |
| GOLO    | label     | goes to a label if the leading integer is smaller than the one before it |
| GOLO    | label     | goes to a label if the leading integer is greater than the one before it |
| GOREQ   | label     | goes to a label if the leading integer is equal to the integer stored at the register |
| HALT    | nil       | exits the program, returns 0 |
| RAND    | int?      | pushes a random integer to the stack, if an integer is provided, uses it as a limit
| RSET    | int       | sets the register to the provided integer |
| RINT    | nil       | increments the value at the register |
| RDEC    | nil       | decrements the value at the register |

  
### Limitations
1. there can be only 256 commands in the script (can be changed in the source file)
2. the stack has a depth limit of 256 ints (also can be changed in the source file)
3. there will be no more than 1024 characters in a line (you know what im about to write)
4. RAND uses rand() to generate a random number

> example scripts are provided in ./scripts if you want to see how things work in practice :)
