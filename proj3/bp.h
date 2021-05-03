#ifndef BP_H
#define BP_H

#include "hash.h"
#include "20141196.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct bp_node {
  int bpLoc;
  struct bp_node* next;
};

struct bp {
  struct bp_node* head;
  struct bp_node* tail;
};

extern struct bp bp[20];

//initialize bp-hash table
void initBp();

//push location into bp container(hash table)
void pushBp(char *loc);

//print all elements of bp table
void printBp();

//clear all elements of bp table
void clearBp();

//determine whether pc is in bp table or not
bool inBp(int pc);

void bpCmd();
#endif