< Project 2 - SIC/XE assembler >

0. How to execute

    + unzipping, then there should be followings.
        - 20141196.c, 20141196.h
        - assemble.c, assemble.h
        - cmdParse.c, cmdParse.h
        - hash.c, hash.h
        - histList.c, histList.h
        - memory.c, memory.h
        - modifyLoc.c, modifyLoc.h
        - opcode.c, opcode.h
        - shell.c, shell.h
        - symbol.c, symbol.h
        - type.c, type.h
        - opcode.txt
        - document.doc
        - 2_5.asm
        - README


    + If you have 11 *.c files, 11 *.h files, opcode.txt, 2_5.asm, then do following steps
        - $ make
        - $ ./20141196.out

    + If you want to cleanup directory (except for original upzipped files), then
        - $ make clean


1. Specification

    + memory size is 1MB.


2. Commands

    - h[elp]
    - d[ir]
    - q[uit]
    - hi[story]
    - du[mp] [start, end]
    - e[dit] address, value
    - f[ill] start, end, value
    - reset
    - opcode mnemonic
    - opcodelist
    - assemble filename
    - type filename
    - symbol

