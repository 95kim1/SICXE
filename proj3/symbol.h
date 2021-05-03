#ifndef SYMBOL_H
#define SYMBOL_H

#include "subFunction.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct sym_node {
  char symbol_[30];
  int loc;
  struct sym_node *next;
};

struct symtab {
  struct sym_node*head;
  struct sym_node*tail;
};

extern struct symtab symtab;
extern struct symtab symtab_backup;

void initSymtab();
int pushSymtab(char *sym, int loc);
int findSymtab(char *sym);
void freeSymtab();
void symbolCmd();
void backupSymtab();
void recoverySymtab();
void freeBackupSymtab();
#endif