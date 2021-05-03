#ifndef HISTLIST_H
#define HISTLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    char *cmd;
    struct node *next;
};

struct hist_list {
    struct node *head;
    struct node *tail;
    int cnt;
};

extern struct hist_list history_list;

void initHistList(); 
char* getCmd(struct node *node);
struct node* nextNode(struct node* node);
struct node* makeNode(char *cmd, struct node *next);
void pushNode(struct node* node);
void freeList();
void printHistList();

#endif