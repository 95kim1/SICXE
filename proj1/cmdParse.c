#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "cmdParse.h"
#include "20141196.h"
#include "shell.h"
#include "memory.h"

/* 목적 : 인자로 받은 address가 알파벳 혹은 숫자가 아닌 문자가 포함됐는지, 메모리 공간을 초과하는지 체크 */
void addressCheck(char *addr, struct input *input) {
    int temp = 0;
    int b = 1;
    for (int i = strlen(addr)-1; i>=0; i--) {
        if ('0' <= addr[i] && addr[i] <= '9') {
            temp += (addr[i]-'0')*b;
        }
        else if ('a' <= addr[i] && addr[i] <= 'z') {
            temp += (addr[i]-87)*b;
        }
        else if ('A' <= addr[i] && addr[i] <= 'Z') {
            temp += (addr[i]-55)*b;
        }
        else {
            input->cmd = error;
            return;
        }
        b*=16;
    }
    if (!(0<=temp&&temp<MEMSIZE)) {
        input->cmd = error;
    }
}

/* 목적 : 인자로 받은 문자열을 통해 명령어의 type을 반환 */
int cmdNum(const char *cmd) {
    if (!strcmp(cmd, "h") || !strcmp(cmd,"help"))
        return help;
    else if (!strcmp(cmd, "d") || !strcmp(cmd, "dir"))
        return dir;
    else if (!strcmp(cmd, "q") || !strcmp(cmd, "quit"))
        return quit;
    else if (!strcmp(cmd, "hi") || !strcmp(cmd, "history"))
        return history;
    else if (!strcmp(cmd, "du") || !strcmp(cmd, "dump"))
        return dump;
    else if (!strcmp(cmd, "e") || !strcmp(cmd, "edit"))
        return edit;
    else if (!strcmp(cmd, "f") || !strcmp(cmd, "fill"))
        return fill;
    else if (!strcmp(cmd, "reset"))
        return reset;
    else if (!strcmp(cmd, "opcode"))
        return opcode;
    else if (!strcmp(cmd, "opcodelist"))
        return opcodelist;

    //printf("Error: wrong command.\n");
    //helpCmd();
    return error;
}

/* 목적 : 명령어를 정제한다. */
struct input parseInput(char *user_input) {
    struct input input;

    char cmd[CMDLEN];
    char temp[CMDLEN] = {0,};
    char args[MAX_ARGS][CMDLEN] = {0,};
    int i = 0, j = 0, k = 0;
    
    //skip white spaces before the user input command
    while(isspace(user_input[i])){i++;}
    // if (isspace(user_input[i])) {
    //     input.cmd = error;
    //     //printf("Error: do not use white space before the command.\n");
    //     return input;
    // }
    
    //ex) "dump 1, 20" -> record "dump" to cmd
    while(!isspace(user_input[i]) && user_input[i]!='\0') {
        cmd[j++] = user_input[i++];
    }
    cmd[j] = '\0';
    
    //skip white spaces btw function and 1st arguement
    //ex) " 1, 20" -> "1, 20"
    while(isspace(user_input[i])){i++;}
    
    //count comma(,)
    int comma_cnt = 0;
    for (j=0;user_input[i]!='\0';j++,i++) {
        temp[j] = user_input[i];
        if (temp[j]==',')
            comma_cnt++;
    }
    
    //record arguments to struct input
    // ex) " 1, 20" -> " 1"  " 20"
    input.arg_cnt = 0;
    i = 0;
    char *ptr;
    if ((ptr=strtok(temp, ","))!=NULL) {
        strcpy(args[i], ptr);
        input.arg_cnt++;
    }
    while(ptr) {
        if ((ptr=strtok(NULL, ","))!=NULL) {
            strcpy(args[++i], ptr);
            input.arg_cnt++;
        }
    }
    
    // cmd의 타입 찾기
    input.cmd = cmdNum(cmd);
    if (input.cmd == error) {
        printf("Error: wrong command.\n");
        return input;
    }

    for (i = 0; i < input.arg_cnt; i++) {
        if (args[i][0] == '\0') break;
        
        j = 0;
        k = 0;
        //ex " 20"->"20"
        while(isspace(args[i][j])){j++;}
        while(!isspace(args[i][j]) && args[i][j] != '\0') {
            input.args[i][k++] = args[i][j++];
        }
        
        // ex dump 1 20 인경우 args[0]에 "1 32"가 저장된다. -> error
        while(isspace(args[i][j])){j++;};
        if (args[i][j]!='\0') {
            printf("Error: wrong arguments.\n");
            input.cmd = error;
            return input;           
        }

        input.args[i][k] = '\0';
    }

    switch(input.cmd) {
        //no argue
        case help:
        case dir:
        case quit:
        case history:
        case reset:
        case opcodelist:
            if (input.arg_cnt > 0 || comma_cnt > 0) {
                printf("Error: wrong arguments.\n");
                input.cmd = error;
                return input;
            }
            break;
        //1 argue
        case opcode:
            if (input.arg_cnt != 1 || comma_cnt > 0) {
                printf("Error: wrong arguments.\n");
                input.cmd = error;
                return input;
            }
            break;
        //2 argues
        case edit:
            if (input.arg_cnt != 2 || comma_cnt != 1) {
                printf("Error: wrong arguments.\n");
                input.cmd = error;
                return input;
            }
            break;
        //3 argues
        case fill:
            if (input.arg_cnt != 3 || comma_cnt != 2) {
                printf("Error: wrong arguments.\n");
                input.cmd = error;
                return input;
            }
            break;
        // 0 or 2 argues
        case dump:
            if (!((input.arg_cnt==0 || input.arg_cnt==1) && comma_cnt==0) && !(input.arg_cnt==2 && comma_cnt==1)) {
                printf("Error: wrong arguments.\n");
                input.cmd = error;
                return input;
            }
            break;
    }

    // printf("---------\n");
    // for (i = 0; i < input.arg_cnt; i++) {
    //     printf("[%s]\n", input.args[i]);
    // }

    // 입력받는 주소 check 0~0xffffff
    for (i = 0; i < input.arg_cnt; i++) {
        //printf("[%s]\n", input.args[i]);
        addressCheck(input.args[i], &input);
        if (input.cmd == error) {
            printf("Error: an address exceeds 0x0FFFFF.\n");
            return input;
        }
    }

    //user input 정제하기.
    //ex  "dump 1  ,  20"  -> "dump 1, 20"
    for (i = 0; i < strlen(cmd); i++){
        user_input[i] = cmd[i];
    }

    for (j = 0; j < input.arg_cnt; j++) {
        user_input[i++] = ' ';
        user_input[i] = '\0';

        strcat(user_input, input.args[j]);
        i+=strlen(input.args[j]);

        if (j!=input.arg_cnt-1) {
            user_input[i++] = ',';
        }   
    }

    user_input[i] = '\0';
    
    //printf("[%s]\n", user_input);
    return input;
}
