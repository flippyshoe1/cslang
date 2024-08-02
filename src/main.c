#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN(a) (sizeof(a) / sizeof(a)[0])
#define ASSERT(_e, ...) { if(!(_e)) {fprintf(stderr, __VA_ARGS__); exit(1);} }

#define CMD_STACK_DEPTH 256
#define PRO_STACK_DEPTH 256
#define CHARACTER_PER_LINE_LIMIT 1024

/* available commands as enums */
typedef enum {
  INVALID=0, // special enum that marks an invalid line
  PUSH, POP, ADD, SUB, PRINT
} opcode;

/* lookup table for enums to cut me some work */
const struct {
  opcode value;
  char *key;
} opcode_lookup[] = {
  {PUSH,  "PUSH"},
  {POP,   "POP"},
  {ADD,   "ADD"},
  {SUB,   "SUB"},
  {PRINT, "PRINT"},
};

// returns -1 if the string isnt a valid enum
opcode
str_to_opcode(char *str){
  for(size_t i=0;i<LEN(opcode_lookup);++i)
    if(!strcmp(str, opcode_lookup[i].key))
      return opcode_lookup[i].value;

  return INVALID;
}

/* the command stack to be used in the interpeter */
struct {
  opcode op;
  char arg[CHARACTER_PER_LINE_LIMIT];
} cmd_stack[CMD_STACK_DEPTH];

static int
first_available_spot(){
  for(int i=0;i<CMD_STACK_DEPTH;++i)
    if(cmd_stack[i].op==INVALID)
      return i;

  return -1;
}

// returns the index of the new command in the stack
int
append_command(char str[]){
  int index = first_available_spot();
  ASSERT(index!=-1, "unable to find free space in the stack\n");

  char *arg;
  if((arg=strchr(str, ' '))!=NULL)
    strcpy(cmd_stack[index].arg, &str[arg-str+1]);
  else
    cmd_stack[index].arg[0]='\0';
  
  char *cmd = strtok(str, " ");
  cmd_stack[index].op = str_to_opcode(cmd);
  ASSERT(cmd_stack[index].op!=INVALID, "invalid command: %s\n",cmd);
  
  //printf("OPCODE:\t%u\nARG:\t%s\n\n", cmd_stack[index].op, cmd_stack[index].arg);

  return index;
}

int
main(int argc, char **argv){
  ASSERT(argc>1, "insufficient amount of args\n");
  
  FILE *script = fopen(argv[1], "r");
  ASSERT(script, "failed to open file at path: %s\n", argv[1]);
  
  size_t rlen;
  char buffer[CHARACTER_PER_LINE_LIMIT];

  // step one: serialise the script to something the computer can read
  while((rlen=fread(buffer, sizeof(char), CHARACTER_PER_LINE_LIMIT, script))>0){
    // get the line in the script, adjust the stream pointer accordingly
    char *line = strtok(buffer, "\n"); // it doesnt point to a literal so things SHOULD be fine... right?
    fseek(script, -(rlen-strlen(line))+1, SEEK_CUR);

    append_command(line);
  }
  
  // step two: go through the serialised product and execute the commands
  int program_stack[256] = {0};
  int *ps_pointer = &program_stack[0];
  
  for(int i=0;i<CMD_STACK_DEPTH;++i){
    if(cmd_stack[i].op==INVALID) // if its invalid we prob reached the last command
      break;

    int res=0;
    switch(cmd_stack[i].op){
      
    case PUSH:
      ps_pointer+=1;
      res=atoi(cmd_stack[i].arg);
      *ps_pointer=res;
      break;
      
    case POP:
      ps_pointer-=1;
      break;

    case ADD:
      res = *(ps_pointer)+*(ps_pointer-1);
      ps_pointer-=1;
      *ps_pointer=res;
      break;

    case SUB:
      res = *(ps_pointer)-*(ps_pointer-1);
      ps_pointer-=1;
      *ps_pointer-=res;
      break;

    case PRINT:
      if(cmd_stack[i].arg[0]!='\0')
	printf("%s\n", cmd_stack[i].arg);
      else
	printf("%d\n", *ps_pointer);

      break;

    default:
      break;
    }
    
  }
  
  fclose(script);
}
