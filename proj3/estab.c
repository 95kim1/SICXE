#include "estab.h"

struct cs_node estab[MAXFILE];

void initEstable() {
  clearEstable();
  for (int i = 0; i < MAXFILE; i++) {
    estab[i].csaddr = 0;
    estab[i].length = 0;
    for (int j = 0; j < 20; j++) {
      estab[i].table[j].addr = -1;
      estab[i].table[j].next = NULL;
    }
  }
}

void clearEstable() {
  struct es_node *node, *temp;
  for (int i = 0; i < MAXFILE; i++) {
    for (int j = 0; j < 20; j++) {
      node = estab[i].table[j].next;
      while(node) {
        temp = node;
        node = node->next;
        free(temp);
      }
    }
  }
}

//input symbol to external symbol table
int inputEstable(char *symbol, int addr, int id) {
  int csaddr = estab[id].csaddr;
  struct es_node *curNode = &estab[id].table[hashFunc(symbol)];

  while(curNode->next) {
    if (strcmp(curNode->next->symbol, symbol)==0)
      return 1; //error
    curNode = curNode->next;
  }

  //make new node
  struct es_node *node = (struct es_node*)malloc(sizeof(struct es_node));
  node->addr = addr + csaddr;
  strcpy(node->symbol, symbol);

  //connect new node
  curNode->next = node;

  return 0;
}

int getAddrBySymbolAndId(char *symbol, int id) {
  struct es_node *curNode = estab[id].table[hashFunc(symbol)].next;
  while(curNode) {
    if (strcmp(curNode->symbol, symbol)==0)
      return curNode->addr;
    curNode = curNode->next;
  }
  return -1;//error no symbol in table
}

int getAddrBySymbol(char *symbol) {
  for (int i = 0; i < input.arg_cnt; i++) {
    int addr = getAddrBySymbolAndId(symbol, i);
    if (addr != -1)
      return addr;
  }
  return -1;
}