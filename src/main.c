#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LEN(a) (sizeof(a) / sizeof(a)[0])
#define ASSERT(_e, ...) { if(!(_e)) {fprintf(stderr, __VA_ARGS__); exit(1);} }

#define CMD_STACK_DEPTH 256
#define PRO_STACK_DEPTH 256
#define CHARACTER_PER_LINE_LIMIT 1024

/* __TODO:__
 * 1. less spaghetti in my code please :)
 * 2. more comments for clarity, most of these functions seem like eldritch magic from a first glance
 * 3. implement more commands, add a stack
 * 4. get rid of res from the switch
 */

/* available commands as enums */
typedef enum {
  INVALID=0, // special enum that marks an invalid line
  PUSH, POP, ADD, SUB,
  PRINT, READ,
  GOTO, GOEQ, GOHI, GOLO, GOREQ,
  HALT, RAND,
  RSET, RINC, RDEC
} opcode;

/* lookup table for enums to cut me some work */
const struct {
  opcode value;
  char *key;
} opcode_lookup[] = {
  {PUSH,  "PUSH" },
  {POP,   "POP"  },
  {ADD,   "ADD"  },
  {SUB,   "SUB"  },
  
  {PRINT, "PRINT"},
  {READ,  "READ" },
  
  {GOTO,  "GOTO" },
  {GOEQ,  "GOEQ" },
  {GOHI,  "GOHI" },
  {GOLO,  "GOLO" },
  {GOREQ, "GOREQ"},
  
  {HALT,  "HALT" },
  {RAND,  "RAND" },

  {RSET,  "RSET" },
  {RINC,  "RINC" },
  {RDEC,  "RDEC" }
};

// returns -1 if the string isnt a valid enum
opcode
str_to_opcode(char *str){
  for(size_t i=0;i<LEN(opcode_lookup);++i)
    if(!strcmp(str, opcode_lookup[i].key))
      return opcode_lookup[i].value;

  return INVALID;
}

opcode
strn_to_opcode(char *str, long len){
  for(size_t i=0;i<LEN(opcode_lookup);++i)
    if(!strncmp(str, opcode_lookup[i].key, len))
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

struct {
  char label[CHARACTER_PER_LINE_LIMIT];
  int i;
} label_stack[CMD_STACK_DEPTH];

static int
first_available_label(){
  for(int i=0;i<CMD_STACK_DEPTH;++i)
    if(label_stack[i].i==0)
      return i;

  return -1;
}

bool
is_command(char str[]){
  char *p = strchr(str, ' ');

  return strn_to_opcode(str, p-&str[0]);
}

bool
is_label(char str[]){
  return str[strlen(str)-1]==':';
}

// returns the index of the new command in the stack
// -1 means no index available
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
string_strip(char *str){
    bool conf = false;
    int index=0;
    for(size_t i=0;i<strlen(str);++i){
        if(str[i]==' '){
            if(conf) {index=i; conf=false;}
            else continue;
        }
        else conf=true;
    }
    if(!conf) str[index]='\0';
    return strlen(str);
}

int
append_label(char str[]){
  int index = first_available_label();
  ASSERT(index!=-1, "unable to find free space in the stack\n");

  str[strlen(str)-1]='\0';
  (void)string_strip(str);
  
  strncpy(label_stack[index].label, str, strlen(str));
  label_stack[index].i=first_available_spot()-1; // in a better world, this would find the command the label is going to point to without subracting 1
                                                 // unfortunately, we dont live in such world.

  return index;
}

int
get_index_from_label(char str[]){
  for(size_t i=0;i<LEN(label_stack);++i){
    if(label_stack[i].i==0)
      break;

    if(!strcmp(str, label_stack[i].label))
      return label_stack[i].i;
  }

  return -1;
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

    if(is_command(line))
      append_command(line);
    else if(is_label(line))
      append_label(line);
    else
      continue;
  }
  
  // step two: go through the serialised product and execute the commands
  int program_stack[256] = {0};
  int program_register = 0;
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

    case READ:
      ps_pointer+=1;
      (void)scanf("%d", &res);
      *ps_pointer=res;
      break;


      // the following two commands do the exact same thing, only one has a condition
    case GOTO:
      (void)string_strip(cmd_stack[i].arg);
      res=get_index_from_label(cmd_stack[i].arg);
      ASSERT(res>-1, "failed to find label: %s\n", cmd_stack[i].arg);
      i=res;
      break;

    case GOEQ:
      if(*(ps_pointer)!=*(ps_pointer-1)) break;
      
      (void)string_strip(cmd_stack[i].arg);
      res=get_index_from_label(cmd_stack[i].arg);
      ASSERT(res>-1, "failed to find label: %s\n", cmd_stack[i].arg);
      i=res;
      break;

    case GOLO:
      if(*(ps_pointer)>=*(ps_pointer-1)) break;
      
      (void)string_strip(cmd_stack[i].arg);
      res=get_index_from_label(cmd_stack[i].arg);
      ASSERT(res>-1, "failed to find label: %s\n", cmd_stack[i].arg);
      i=res;
      break;

    case GOHI:
      if(*(ps_pointer)<=*(ps_pointer-1)) break;
      
      (void)string_strip(cmd_stack[i].arg);
      res=get_index_from_label(cmd_stack[i].arg);
      ASSERT(res>-1, "failed to find label: %s\n", cmd_stack[i].arg);
      i=res;
      break;

    case GOREQ:
      if(*(ps_pointer)!=program_register) break;
      
      (void)string_strip(cmd_stack[i].arg);
      res=get_index_from_label(cmd_stack[i].arg);
      ASSERT(res>-1, "failed to find label: %s\n", cmd_stack[i].arg);
      i=res;
      break;
      
    case HALT:
      exit(0);
      break; // it wont ever reach here im just adding this for formality

    case RAND:
      ps_pointer+=1;
      // i dare you to write something uglier
      res=(cmd_stack[i].arg[0]!='\0')?rand()%atoi(cmd_stack[i].arg):rand();
      *ps_pointer=res;
      break;

    case RSET:
      program_register=atoi(cmd_stack[i].arg);
      break;

    case RINC:
      program_register+=1;
      break;

    case RDEC:
      program_register-=1;
      break;
      
    default:
      break;
    }
  }
  
  fclose(script);
}
