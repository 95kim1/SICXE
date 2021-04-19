#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "20141196.h"
#include "cmdParse.h"
#include "histList.h"
#include "memory.h"
#include "hash.h"
#include "opcode.h"
#include "assemble.h"
#include "type.h"
#include "symbol.h"

struct input input;

/* 목적 : 초기화 */
void init() {
    //initialize
    initHistList();    //init history list
    resetCmd();                     //init memory
    opcodeInit();                     //init hash table by opcode.txt (fail: init_hash==0, success: init_hash==1)
    initSymtab();
}

/* 목적 : 프로그램 시작 */
int main(void) {
    char end_flag = FALSE;      //check that a user wants to quit
    char user_input[CMDLEN];    //user's input cmd storage

    //initialize
    init();

    while(!end_flag) {
        printf("sicsim> ");
        
        fgets(user_input, CMDLEN, stdin);
        user_input[strlen(user_input)-1] = '\0'; //\n->\0
        
        input = parseInput(user_input);

        if (input.cmd == error) {
            continue;
        }

        switch(input.cmd) {
            //for shell
            case help: //enum type
                helpCmd();
                break;
            case dir:
                dirCmd();
                break;
            case quit:
                quitCmd();
                end_flag = TRUE;
                break;
            case history:
                historyCmd(user_input);
                printHistList();
                break;
            
            //for memory
            case dump:
                dumpCmd();
                break;
            case edit:
                editCmd();
                break;
            case fill:
                fillCmd();
                break;
            case reset:
                resetCmd();
                break;
            
            //for opcode
            case opcode_: {
                int err = opcodeCmd();
                if (err<0) {
                    input.cmd = error;
                    if (err==-1)
                        printf("Error: wrong opcode.txt\n");
                    else
                        printf("Error: wrong argument.\n");
                }
                break;
            }
            case opcodelist:
                opcodelistCmd();
                break;
            case type_:
                typeCmd();
                break;
            case assemble:
                assembleCmd();
                break;
            case symbol:
                symbolCmd();
                break;
        }

        if (input.cmd != error && input.cmd != history)
            historyCmd(user_input);
    }

    return 0;
}
