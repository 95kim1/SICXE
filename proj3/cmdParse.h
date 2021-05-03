#ifndef CMDPARSE_H
#define CMDPARSE_H

#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include "20141196.h"
#include "shell.h"
#include "memory.h"

int cmdNum(const char *cmd);
struct input parseInput(char *user_input);

#endif