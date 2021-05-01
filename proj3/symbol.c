#include "symbol.h"
#include "subFunction.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct symtab symtab;
struct symtab symtab_backup;

void initSymtab() {
  symtab.head = NULL;
  symtab.tail = NULL;
}

//no duplication : return 1, duplitcation : return 0;
int pushSymtab(char *sym, int loc) {
  struct sym_node *temp_node = (struct sym_node*)malloc(sizeof(struct sym_node));
  temp_node->loc = loc;
  temp_node->next = NULL;
  strcpy(temp_node->symbol_, sym);

  struct sym_node *cur = symtab.head;  
  if (cur==NULL) {
    symtab.head = temp_node;
    symtab.tail = temp_node;
    return 1;
  }

  int cmp = strcmp(sym, cur->symbol_);
  if (cmp < 0) {
    temp_node->next = cur;
    symtab.head = temp_node;
    return 1;
  }
  else if (cmp == 0) {
    free(temp_node);
    return 0;
  }
  
  cmp = strcmp(sym, cur->next->symbol_);
  while(cur->next != NULL && (cmp > 0)) {
    cur = cur->next;
    if (cur->next)
      cmp = strcmp(sym, cur->next->symbol_);
  }

  if (cur->next && cmp == 0) {
    free(temp_node);
    return 0;
  }

  temp_node->next = cur->next;
  cur->next = temp_node;
  return 1;
}

/*find : return location, no : return -1*/
int findSymtab(char *sym) {
  struct sym_node *cur = symtab.head;
  while(cur != NULL && !(strcmp(sym, cur->symbol_)==0)) {
    cur = cur->next;
  }
  if (cur == NULL) {
    return -1;
  }
  return cur->loc;
}

void freeSymtab() {
  struct sym_node *cur = symtab.head;
  while(cur != NULL) {
    struct sym_node *temp_node = cur;
    cur = cur->next;
    free(temp_node);
  }
  symtab.tail = symtab.head = NULL;
}

/*print all contents in symtab*/
void symbolCmd() {
  struct sym_node *cur = symtab.head;
  if (cur == NULL) return;
  char hex[10];
  while(cur->next != NULL) {
    itoh(cur->loc, hex, 4);
    printf("%s\t%s\n", cur->symbol_, hex);
    cur = cur->next;
  }
  itoh(cur->loc, hex, 4);
  printf("%s\t%s\n", cur->symbol_, hex);
}

void backupSymtab() {
  symtab_backup.head = symtab.head;
  symtab_backup.tail = symtab.tail;
  initSymtab();
}

void recoverySymtab() {
  freeSymtab();
  symtab.head = symtab_backup.head;
  symtab.tail = symtab_backup.tail;
}

void freeBackupSymtab() {
  struct sym_node *cur = symtab_backup.head;
  while(cur != NULL) {
    struct sym_node *temp_node = cur;
    cur = cur->next;
    free(temp_node); 
  }
  symtab_backup.tail = symtab_backup.head = NULL;
}