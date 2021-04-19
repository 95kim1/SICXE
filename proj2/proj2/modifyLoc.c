#include <stdlib.h>
#include "modifyLoc.h"

void initModifyList(struct modifyList *modifyList) {
  modifyList->head = modifyList->tail = NULL;
  modifyList->nextIdx = 0;
}

struct modifyNode* makeModifyNode(int idx, struct modifyNode *next) {
  struct modifyNode* tempNode = (struct modifyNode*)malloc(sizeof(struct modifyNode));
  tempNode->idx = idx;
  tempNode->next = next;
  return tempNode;
}

void pushModifyNode(struct modifyList *modifyList, struct modifyNode *modifyNode) {
  if (modifyList->head == NULL) {
    modifyList->head = modifyList->tail = modifyNode;
    return;
  }
  modifyList->tail->next = modifyNode;
  modifyList->tail = modifyNode;
}

void freeModifyList(struct modifyList *modifyList) {
  struct modifyNode *cur = modifyList->head, *temp;
  while(cur!=NULL) {
    temp = cur;
    cur = cur->next;
    free(temp);
  }
  initModifyList(modifyList);
}