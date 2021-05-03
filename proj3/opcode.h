#ifndef OPCODE_H
#define OPCODE_H

#include "hash.h"
#include "20141196.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//proj1
int opcodeCmd();
void opcodelistCmd();
void opcodeInit();

//proj2
struct hash_node* getOpcode(char* mnemonic, char* copyDest);

#endif