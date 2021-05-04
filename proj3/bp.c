#include "bp.h"

struct bp bp[20];

void initBp() {
  for (int i = 0; i < 20; i++)
    bp[i].tail = bp[i].head = NULL;
}

void pushBp(char *loc) {
  char hex[7];
  itoh(htoi(loc), hex, 6);

  int idx = hashFunc(hex);

  struct bp_node* node = (struct bp_node*)malloc(sizeof(struct bp_node));
  node->next = NULL;
  node->bpLoc = htoi(hex);

  if (bp[idx].head == NULL) {
    bp[idx].head = bp[idx].tail = node;
    return;
  }

  bp[idx].tail->next = node;
  bp[idx].tail = node;
}

void printBp() {
  printf("            breakpoint\n");
  printf("            ----------\n");
  for (int i = 0; i < 20; i++) {
    if (bp[i].head==NULL)
      continue;
    struct bp_node* cur = bp[i].head;
    while(cur) {
      printf("            %X\n", cur->bpLoc);
      cur = cur->next;
    }
  }
}

void clearBp() {
  for (int i = 0; i < 20; i++) {
    struct bp_node* cur = bp[i].head, *temp;
    while(cur) {
      temp = cur;
      cur = cur->next;
      free(temp);
    }
    bp[i].head = bp[i].tail = NULL;
  }
}

bool inBp(int pc) {
  char hex[7];
  itoh(pc, hex, 6);

  int idx = hashFunc(hex);

  struct bp_node* cur = bp[idx].head;
  
  while(cur) {
    if (cur->bpLoc == pc)
      return true;
      
    cur = cur->next;
  }

  return false;
}

void bpCmd() {
  int bp = htoi(input.args[0]);
  
  if (input.arg_cnt == 0)
    printBp();
  else if (strcmp(input.args[0], "clear")==0) {
    clearBp();
    printf("            [ok] clear all breakpoints\n");
  }
  else {
    pushBp(input.args[0]);
    printf("            [ok] create breakpoint %x\n", bp);
  }
}