#ifndef BASE
#define BASE

#include <ctype.h>
#define TRUE 1
#define FALSE 0
#define CMDLEN 128
#define MAX_ARGS 3
#define ARGLEN 15

enum cmd {
    error,                      //wrong cmd
    help, dir, quit, history,   //relate to shell
    dump, edit, fill, reset,    // to memory
    opcode, opcodelist,         // to opcode
};

struct input {
    int cmd;
    int arg_cnt;
    char args[MAX_ARGS][ARGLEN];
};

struct input input;

#endif