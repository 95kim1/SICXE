#ifndef BASE_H
#define BASE_H

#include <ctype.h>
#define TRUE 1
#define FALSE 0
#define CMDLEN 128
#define MAX_ARGS 3
#define ARGLEN 30

enum cmd {
    error,                          //wrong cmd
    help, dir, quit, history,       //relate to shell
    dump, edit, fill, reset,        // to memory
    opcode_, opcodelist,            // to opcode
    type_, assemble, symbol,        // proj2
    progaddr_, loader_, run_, bp_,  // proj3
};

struct input {
    int cmd;
    int arg_cnt;
    char args[MAX_ARGS][ARGLEN];//operands
};

extern struct input input;

#endif