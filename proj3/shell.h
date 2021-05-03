#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>

#include "histList.h"

#include "histList.h"
#include "20141196.h"
#include "hash.h"

void helpCmd();     //print cmd help
void dirCmd();      //print list of current directory.
void quitCmd();     //quit program (free dynamic allocations)
void historyCmd(char *user_input);  //print history

#endif