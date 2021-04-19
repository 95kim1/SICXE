#include "histList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 목적 : history list 초기화 */
void initHistList()
{
    history_list.cnt = 0;
    history_list.head = NULL;
    history_list.tail = NULL;
}

/* 목 적 : node에 저장된 명령어 반환 */
char* getCmd(struct node *node)
{
    return node->cmd;
}

/* 목적 : 현재 노드의 다음 노드를 반환 */
struct node* nextNode(struct node* node)
{
    return node->next;
}

/* 목적 : 노드를 새로 만들어서 반환 */
struct node* makeNode(char *cmd, struct node *next)
{
    struct node* node = (struct node*)malloc(sizeof(struct node));
    node->cmd = (char*)malloc(strlen(cmd)+1);
    strcpy(node->cmd, cmd);
    node->next = next;
    return node;
}

/* 목적 : history list에 노드를 삽입한다. */
void pushNode(struct node* node)
{
    (history_list.cnt)++;
    if (history_list.tail == NULL) {
        history_list.tail = node;
        history_list.head = node;
    }
    else {
        history_list.tail->next = node;
        history_list.tail = node;
    }
}

/* 목적 : 동적할당된 공간 해제 */
void freeList()
{
    history_list.cnt = 0;
    struct node *cur = history_list.head;
    struct node *temp;
    while(cur) {
        temp = cur;
        cur = nextNode(cur);
        free(temp->cmd);
        free(temp);
    }
}

/* 목적 : history list의 모든 노드를 탐색하며 출력 */
void printHistList() {
    struct node*cur = history_list.head;
    int cnt = 0;
    while(cur) {
        cnt++;
        printf("%-6d%s\n", cnt, cur->cmd);
        cur = nextNode(cur);
    }
}