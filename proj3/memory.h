#ifndef MEMORY_H
#define MEMORY_H

#include "20141196.h"
#define MEMSIZE (1<<20)

enum REG {
  A, X, L, //0, 1, 2,
  B, S, T, F, NONE, //3, 4, 5, 6
  PC, SW //8, 9
};

extern int reg[10];

extern char mem[MEMSIZE];

void dumpCmd();
void fillCmd();
void editCmd();
void resetCmd();

//proj3
void initMemory();
void initReg();

#endif