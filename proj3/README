< Project 2 - SIC/XE linking loader >

0. How to execute

+ unzipping, then there should be followings.
    - 20141196.c, 20141196.h
    - assemble.c, assemble.h
    - bp.c, bp.h
    - cmdParse.c, cmdParse.h
    - estab.c, estab.h
    - hash.c, hash.h
    - histList.c, histList.h
    - load.c, load,h
    - memory.c, memory.h
    - modifyLoc.c, modifyLoc.h
    - opcode.c, opcode.h
    - run.c, run.h
    - shell.c, shell.h
    - symbol.c, symbol.h
    - type.c, type.h
    - opcode.txt
    - document.doc
    - 2_5.asm
    - copy.obj
    - proga.obj, progb.obj, progc.obj
    - README
    - Makefile


+ If you have 15 *.c files, 15 *.h files, opcode.txt, 2_5.asm, proga.obj, progb.obj, progc.obj, copy.obj then do following steps
    - $ make
    - $ ./20141196.out

+ If you want to cleanup directory (except for original upzipped files), then
    - $ make clean


1. Specification

+ memory size is 1MB.
+ a few assembly directives are not implemented
+ a few instructions are not implemented


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
- progaddr address
- loader file1 [file2 [file3]]
- bp [address or clear]
- run

