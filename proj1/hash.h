#ifndef HASH
#define HASH

struct hash_node {
    char op[7];
    char opNum[3];
    char format[4];
    struct hash_node *next;
};

struct hash_node hash_table[20];
int hash_init;

void initHash();
void freeHash();
int hashFunc(const char *op);
#endif