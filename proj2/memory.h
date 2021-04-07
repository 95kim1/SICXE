#ifndef MEMORY_H
#define MEMORY_H

#include "20141196.h"
#define MEMSIZE (1<<20)

char mem[MEMSIZE];

void dumpCmd();
void fillCmd();
void editCmd();
void resetCmd();

#endif