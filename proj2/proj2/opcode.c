#include "opcode.h"
#include "hash.h"
#include "20141196.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* 목적 : mnemonic에 대응되는 opcode 출력, 정상 출력 시 0을 반환, error 발생 시 -2 or -1 반환  */
int opcodeCmd() {
    for (int i = 0; i < strlen(input.args[0]); i++) {
        if (!('A' <= input.args[0][i] && input.args[0][i] <= 'Z')) {
            return -2; //wrong argument
        }
    }

    int index = hashFunc(input.args[0]);
    struct hash_node *node = hash_table[index].next;
    while(node) {
        if (strcmp(node->op, input.args[0])==0) {
            printf("opcode is %s\n", node->opNum);
            return 0;
        }
        node = node->next;
    }
    return -1; //wrong opcode.txt
}

/* 목적 : 모든 mnemonic과 그에 대응되는 opcode 출력 */
void opcodelistCmd() {
    for (int i = 0; i < 20; i++) {
        printf("%d : ", i);
        struct hash_node *node = hash_table[i].next;
        while(node) {
            printf("[%s, %s]", node->op, node->opNum);
            if (node->next)
                printf(" -> ");
            node = node->next;
        }
        printf("\n");
    }
}

/* 목적 : opcode.txt를 읽어서 mnemonic을 key 값으로 하고 opcdoe를 value로 하는 hash table 만들기 */
void opcodeInit() {
    initHash();

    FILE *fp = fopen("./opcode.txt", "r");
    if (fp == NULL) {
        hash_init = 0;
        return;
    }

    hash_init = 1;
    char opNum[3];
    char op[7];
    char format[4];
    char str[128];
    char temp_str[10];

    while(fgets(str, 128, fp) != NULL) {
        str[strlen(str)-1] = '\0';
        int i = 0, j, cnt = 0;
        while(cnt++ < 3) {
            j = 0;
            while(!isspace(str[i]) && str[i] != '\0') {
                temp_str[j++] = str[i++];
            }
            temp_str[j] = '\0';

            switch(cnt) {
                case 1:
                    strcpy(opNum, temp_str);
                    break;
                case 2:
                    strcpy(op, temp_str);
                break;
                case 3:
                    strcpy(format, temp_str);
                break;
            }

            while(isspace(str[i])) {i++;}
        }
        
        int index = hashFunc(op);
        struct hash_node* node = hash_table[index].next;
        //printf("%d : [%d, %s, %s]\n", index, opNum, op, format);
        
        struct hash_node* temp = (struct hash_node*)malloc(sizeof(struct hash_node));
        temp->next = NULL;
        strcpy(temp->opNum, opNum);
        strcpy(temp->op, op);
        strcpy(temp->format, format);

        if (node==NULL) {
            hash_table[index].next = temp;
        }
        else {
            while(node->next) {
                node = node->next;
            }
            node->next = temp;
        }
    }

    fclose(fp);
}

//proj2
// Is a mnemonic argument in hash_table(OPTAB)?
// yes: copy and return hash_node address, no: return NULL
struct hash_node* getOpcode(char*mnemonic, char *copyDest) {
    int index = hashFunc(mnemonic);
    struct hash_node *node = hash_table[index].next;
    while(node) {
        if (strcmp(node->op, mnemonic)==0) {
            if (copyDest)
                strcpy(copyDest, node->opNum);
            return node;
        }
        node = node->next;
    }
    return NULL;
}