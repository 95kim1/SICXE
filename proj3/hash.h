#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hash_node {
    char op[7];
    char opNum[3];
    char format[4];
    struct hash_node *next;
};

extern struct hash_node hash_table[20];
extern int hash_init;

void initHash();
void freeHash();
int hashFunc(const char *op);
#endif