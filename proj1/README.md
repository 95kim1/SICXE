# < Project 1 - SIC/XE shell prompt >

0. How to execute

    + unzipping, then there should be followings.
        - 20141196.c, 20141196.h
        - cmdParse.c, cmdParse.h
        - hash.c, hash.h
        - histList.c, histList.h
        - memory.c, memory.h
        - opcode.c, opcode.h
        - shell.c, shell.h
        - opcode.txt
        - document.doc
        - README

    + If you have 7 *.c files, 7 *.h files, opcode.txt, then do following steps
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

