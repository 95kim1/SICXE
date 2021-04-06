#ifndef SHELL
#define SHELL
#include "histList.h"
void helpCmd();     //print cmd help
void dirCmd();      //print list of current directory.
void quitCmd();     //quit program (free dynamic allocations)
void historyCmd(char *user_input);  //print history

#endif