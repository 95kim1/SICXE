#include "memory.h"
#include <stdio.h>
#include "20141196.h"
#include "string.h"

/* 목적 : mem 전역변수 초기화, 메모리 공간 0으로 초기화 */
void resetCmd() {
    for (int i = 0; i < MEMSIZE; i++)
        mem[i] = 0;
}

/* 목적 : string형태의 16진수를 int로 변환 */
void hToi(char *hex, int *address) {
    *address = 0;
    int b = 1;
    for (int i = strlen(hex)-1; i >= 0; i--) {
        if ('0' <= hex[i] && hex[i] <= '9')
            *address += (hex[i]-'0')*b;
        else if ('a' <= hex[i] && hex[i] <= 'z')
            *address += (hex[i]-87)*b;
        else if ('A' <= hex[i] && hex[i] <= 'Z')
            *address += (hex[i]-55)*b;
        else {
            input.cmd = error;
            return;
        }
        b*=16; 
    }
}

/* 목적 : 메모리 공간 16byte를 출력 */
void dumpOneLine(int start, int end, int base) {
    int base__ = base;
    char hexAddr[7];
    for (int i = 5; i >= 0; i--) {
        int temp = base__ % 16;
        if (temp < 10)
            hexAddr[i] = temp + '0';
        else
            hexAddr[i] = temp + 55;
        base__/=16;
    }
    hexAddr[6] = '\0';
    printf("%s ", hexAddr);

    for (int i = 0; i < 16; i++) {
        if (base+i < start || base+i > end)
            printf("   ");
        else {
            char front = (mem[base+i]>>4) & 15;
            front = (front < 10) ? front+'0' : front+55;
            char back = (mem[base+i]&15);
            back = (back < 10) ? back+'0' : back+55;
            printf("%c%c ", front, back);
        }
    }
    printf("; ");
    for (int i = 0; i < 16; i++) {
        if (0x20 <= mem[base+i] && mem[base+i]<= 0x7E)
            printf("%c", mem[base+i]);
        else
            printf(".");
    }
    printf("\n");
}

/* 목적 : 사용자로부터 입력 받은 출력 범위의 메모리 주소를 출력 */
void dumpCmd() {
    static int address = 0;
    int start, end;
    
    if (input.arg_cnt==0) {
        start = address;
        end = start + 16*10 - 1;
    }
    else {
        hToi(input.args[0], &start);
        if (input.arg_cnt>1) {
            hToi(input.args[1], &end);
        }
        else {
            end = start + 16*10 - 1;
        }
    }

    if (input.cmd == error) {
        printf("Error: wrong argument.\n");
        return;
    }

    if (start >= MEMSIZE) {
        printf("Error: exceed maximum address.\n");
        input.cmd = error;
        return;
    }

    if (start > end) {
        printf("Error: start is bigger than end.\n");
        input.cmd = error;
        return;
    }

    //if end exceed, then change end
    if (end/16 - start/16 >= 10) {
        end = (start/16 + 10) * 16 - 1;
        if (end >= MEMSIZE)
            end = MEMSIZE-1;
    }

    //printf("[%d %d %d ], %d\n", start, end, address, input->arg_cnt);

    int addr = start / 16 * 16;
    while(addr <= end) {
        //printf("%d %d %d\n", start, end, addr);
        dumpOneLine(start, end, addr);
        addr += 16;
    }

    address = end + 1;
    if (address >= MEMSIZE)
        address = 0;
}

/* 목적 : 원하는 메모리 주소에 원하는 값을 채워 넣는다. */
void editCmd() {
    int addr, val;
    hToi(input.args[0], &addr);
    hToi(input.args[1], &val);
    if (input.cmd == error) {
        printf("Error: wrong argument.\n");
        return;
    }
    
    if (val >= 256) {
        input.cmd = error;
        printf("Error: exceed maximum value.\n");
        return;
    }

    mem[addr] = val;
}

/* 목적 : 원하는 메모리 주소 범위에 원하는 값을 채워 넣는다. */
void fillCmd() {
    int start, end, val;
    hToi(input.args[0], &start);
    hToi(input.args[1], &end);
    hToi(input.args[2], &val);

    if (input.cmd == error) {
        printf("Error: wrong argument.\n");
        return;
    }

    if (val >= 256) {
        input.cmd = error;
        printf("Error: exceed maximum value.\n");
        return;
    }

    for (int i = start; i <= end && i < MEMSIZE; i++) {
        mem[i] = val;
    }
}
