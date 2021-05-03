#ifndef ESTAB_H
#define ESTAB_H

#include "hash.h"
#include "20141196.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAXFILE 3

struct es_node {
  char symbol[30];
  int addr;
  struct es_node* next;
};

struct cs_node {
  int csaddr;
  int length;
  struct es_node table[20];
};

extern struct cs_node estab[MAXFILE];

void initEstable();

void clearEstable();

//input symbol to external symbol table
int inputEstable(char *symbol, int addr, int id);

//return address of symbol
int getAddrBySymbolAndId(char *symbol, int id);
int getAddrBySymbol(char *symbol);
#endif