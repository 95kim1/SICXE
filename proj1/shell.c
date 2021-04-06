#include "shell.h"
#include "ctype.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "histList.h"
#include "20141196.h"
#include "hash.h"

/* 목적 : 유저에게 명령어 입력 형식을 알려준다. */
void helpCmd() {
    printf("h[elp]\n"
    "d[ir]\n"
    "q[uit]\n"
    "hi[story]\n"
    "du[mp] [start, end]\n"
    "e[dit] address, value\n"
    "f[ill] start, end, value\n"
    "reset\n"
    "opcode mnemonic\n"
    "opcodelist\n");
}

/* 목적 : 현재 위치 디렉토리의 리스트를 출력해준다. */
void dirCmd() {
    char path[258] = "./\0"; //리눅스 파일 이름 길이 : 255 (./와 \0을 추가하여 최종 258 bytes)
    DIR *dir;
    struct dirent *sdir;
    struct stat st;
    
    if ((dir=opendir(".")) == NULL) {
        printf("Error: Unable to open directory.\n");
        return;
    }

    while((sdir=readdir(dir)) != NULL) {
        path[2] = '\0';
        stat(strcat(path,sdir->d_name), &st);
        
        printf("%s", sdir->d_name);
        if (S_ISDIR(st.st_mode))
            printf("/\n");
        else if (st.st_mode & S_IXUSR)
            printf("*\n");
        else
            printf("\n");
    }

    closedir(dir);
}

/* 목적 : 각종 동적 할당된 공간을 해제한다. */
void quitCmd() {
    freeList();
    freeHash();
}

/* 목적 : history list에 정상 입력된 명령어를 저장한다. */
void historyCmd(char *user_input) {
    pushNode(makeNode(user_input, NULL));
}