## CSLang - C Stack (based) Language
(note: this project is practically useless and is only used for me to learn how to write programming languages)

CSlang is a stack based "programming" ""language"" written in C\
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
- PUSH <int>
  - pushes the int to the stack
- POP
  - gets rid of the leading int in the stack
- ADD/SUB
  - adds/subtracts from the last 2 ints in the stack, pushes the result
- PRINT <string?>
  - if an arg is provided, print will well, print it to stdout, otherwise it will print the leading int in the stack
