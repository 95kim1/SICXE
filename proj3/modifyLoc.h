#ifndef MODIFYLOC_H
#define MODIFYLOC_H

//record format 4 object code's index in objFile
struct modifyNode {
  int idx;
  struct modifyNode *next;
};
struct modifyList {
  int nextIdx;
  struct modifyNode *head;
  struct modifyNode *tail;
};

void initModifyList(struct modifyList *modifyList);
struct modifyNode* makeModifyNode(int idx, struct modifyNode *next);
void pushModifyNode(struct modifyList *modifyList, struct modifyNode *modifyNode);
void freeModifyList(struct modifyList *modifyList);

#endif