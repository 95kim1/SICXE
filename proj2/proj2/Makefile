20141196.out : 20141196.o shell.o cmdParse.o histList.o memory.o hash.o opcode.o type.o assemble.o symbol.o subFunction.o modifyLoc.o
	gcc -Wall  -o 20141196.out 20141196.o shell.o cmdParse.o histList.o memory.o hash.o opcode.o type.o assemble.o symbol.o subFunction.o modifyLoc.o

20141196.o : 20141196.c
	gcc -Wall  -c -o 20141196.o 20141196.c

shell.o : shell.c
	gcc -Wall  -c -o shell.o shell.c

cmdParse.o : cmdParse.c
	gcc -Wall  -c -o cmdParse.o cmdParse.c

histList.o : histList.c
	gcc -Wall  -c -o histList.o histList.c

memory.o : memory.c
	gcc -Wall  -c -o memory.o memory.c

hash.o : hash.c
	gcc -Wall  -c -o hash.o hash.c

opcode.o : opcode.c
	gcc -Wall  -c -o opcode.o opcode.c

type.o : type.c
	gcc -Wall  -c -o type.o type.c

assemble.o : assemble.c
	gcc -Wall  -c -o assemble.o assemble.c

symbol.o : symbol.c
	gcc -Wall  -c -o symbol.o symbol.c

subFunction.o : subFunction.c
	gcc -Wall  -c -o subFunction.o subFunction.c

modifyLoc.o : modifyLoc.c
	gcc -Wall  -c -o modifyLoc.o modifyLoc.c

clean:
	rm *.o 20141196.out *.list *.obj
