#include "hash.h"

struct hash_node hash_table[20];
int hash_init;

/* 목적 : hash값 구하기 */
int hashFunc(const char *op) {
    unsigned long long hash = 19273; //소수
    for (int i = 0; i < strlen(op); i++){ 
        hash = (hash<<(op[i]%7)) + hash + op[i];
    }
    
    return hash % 20;
}

/* 목적 : hash table 초기화 */
void initHash() {
    for (int i = 0; i < 20; i++) {
        hash_table[i].next = NULL;
    }
}

/* 목적 : hash table 동적 할당된 공간 해제 */
void freeHash() {
    for (int i = 0; i < 20; i++) {
        struct hash_node * node = hash_table[i].next;
        while(node) {
            struct hash_node *temp = node;
            node = node->next;
            free(temp);
        }
        hash_table[i].next = NULL;
    }
}
