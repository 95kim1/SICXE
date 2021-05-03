#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "modifyLoc.h"
#include "20141196.h"
#include "opcode.h"
#include "subFunction.h"
#include "hash.h"
#include "symbol.h"
#include "memory.h"

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct intermediateFileLine {
  int line;
  int loc;
  int type;
  char label[30];
  char mnemo[30];
  int format;
  char operand1[30];
  char operand2[30];
};

enum typeNum {
  COMMENT_,
  
  SYMBOL_,

  START_,
  END_,

  MNEMO_,
  
  BASE_,
  BYTE_,
  WORD_,
  RESB_,
  RESW_,
};

//tell apart a LINE whether COMMENT or not
int isComment(char *lineString);

// check types(START END MNEONIC BASE BYTE RESB ...) for each word tokenized from a LINE
void checkTypes(char tokens[][30], int size, int typeNums[]);

// tokenize lineString
// reutrn cnt (of tokenized strings)
int tokenize(char *lineString, char tokens[][30]);

// asm file line error check
// error: return 1, no error: return 0;
int isErrorLine(int typeNums[], int size);

// parse one line of asm file
// return count of words (But, error -> return -1)
int parseLine(char *lineString, char tokens[][30], int typeNums[]);

// read file
// error: return NULL
char* getLine(char *lineString, int size, FILE* fp);

// return a type of line (START, END, MNEMONIC, BASE, BYTE, ...)
int whichType(int typeNums[], int size, int *idx);


// copy between two lines of intermediate file
void copyLine(struct intermediateFileLine *dest, struct intermediateFileLine *src);

// read a line from intermediate file
void getLineOfIntermediateFile(struct intermediateFileLine *lineInfo, FILE *fp);

// remove extension from filename
void getPureFileName(char *filename, char *fullname);

// write starting line of .obj file
void writeStartingOfObjFile(FILE *fp, char *programName);

//copy: from src to dest (without a character, src[0])
void removeFrontChar(char *dest, char *src);

// register string to number
int whichReg(char *regStr);

// make object code and return object code's length (in byte unit)
int makeObjectCodeForMnemonic(struct intermediateFileLine *curLine, char*objCode);

// record object code line to obj file 
// if objCodeLine is full enough or location is too far,
// else record object code to objCodeLine
void writeObjFileLine(FILE *fpObj, char *objCodeLine, char *objCode, int *startLocObjLine, int curLoc, struct modifyList *modifyList, char plus);

void assemblePass1();
void assemblePass2();
void assembleCmd();

#endif