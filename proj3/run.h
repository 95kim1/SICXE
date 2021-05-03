#ifndef RUN_H
#define RUN_H

#include "memory.h"
#include "bp.h"

#include <stdbool.h>

extern int startAddr;
extern int endAddr;

typedef enum {
  ADD=0x18,
  ADDF=0x58,
  ADDR=0x90,
  AND=0x40,
  
  CLEAR=0xB4,
  COMP=0x28,
  COMPF=0x88,
  COMPR=0xA0,
  
  DIV=0x24,
  DIVF=0x64,
  DIVR=0x9C,

  FIX=0xC4,
  FLOAT=0xC0,

  HIO=0xF4,

  J=0x3C,
  JEQ=0x30,
  JGT=0x34,
  JLT=0x38,
  JSUB=0x48,

  LDA=0x00,
  LDB=0x68,
  LDCH=0x50,
  LDF=0x70,
  LDL=0x08,
  LDS=0x6C,
  LDT=0x74,
  LDX=0x04,
  LPS=0xD0,

  MUL=0x20,
  MULF=0x60,
  MULR=0x98,

  NORM=0xC8,

  OR=0x44,

  RD=0xD8,
  RMO=0xAC,
  RSUB=0x4C,

  SHIFTL=0xA4,
  SHIFTR=0xA8,
  SIO=0xF0,
  SSK=0xEC,
  STA=0x0C,
  STB=0x78,
  STCH=0x54,
  STF=0x80,
  STI=0xD4,
  STL=0x14,
  STS=0x7C,
  STSW=0xE8,
  STT=0x84,
  STX=0x10,
  SUB=0x1C,
  SUBF=0x5C,
  SUBR=0x94,
  SVC=0xB0,

  TD=0xE0,
  TIO=0xF8,
  TIX=0x2C,
  TIXR=0xB8,

  WD=0xDC,
} MNEMONIC;

typedef enum {
  FORMAT_SIC, FORMAT1, FORMAT2, FORMAT3, FORMAT4,
} FORMAT;

typedef enum {
  SIC, IMMEDIATE, INDIRECT, SIMPLE,
} ADDRESS_MODE;

typedef enum {//SW register value
  LT=-1, EQ=0, GT=1,
} CC;

typedef union {
  int reg[2];//register number
  int TA;//target address
} OPERAND;

struct OBJ_CODE {
  MNEMONIC mnemonic;
  FORMAT format;
  ADDRESS_MODE addrMode;
  OPERAND operand;
};

//get format of mnemonic
int getFormat(int mnemonic);

//parse object code
void parseObjCode(int pc, struct OBJ_CODE* obj);

//set CC(condition code)
void setCC(int cmp);

//set float to the F register
void setF(long long res);

//get float from the F register
long long getF();

//get Value in the location, TA
//cnt: the number of bytes
long long getValue(int TA, int cnt);

//set target value with register
//cnt: the number of bytes, rnum: registere number
void setValue(int TA, int cnt, int rnum);

//get target value
//cnt: the number of bytes
//addrMode: simple, indirect, immediate, SIC
long long getTargetVal(int addrMode, int TA, int cnt);

//execute cmd(object code)
void cmdExecute(struct OBJ_CODE obj);

void runCmd();

#endif