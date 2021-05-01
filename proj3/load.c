#include "load.h"
#include "estab.h"
#include "subFunction.h"
#include "20141196.h"
#include "assemble.h"
#include "memory.h"
#include <string.h>
#include <stdio.h>

int progaddr;

//get certain record (ex: C=='H' -> read head record,  C=='T' -> read text record)
int getCertainLine(char*lineStr, int size, FILE *fp, char C) {
  do {
    if (getLine(lineStr, size, fp)==NULL) {
      input.cmd = error;
      return 0;
    }
  } while(lineStr[0] != C);
  return 1;
}

// stroe external symbol (in obj files) to ESTAB
void createEstab(char *file, int id, char table[][7][30]) {//id : file id (id-th order of operand)
  char lineStr[80];
  char lenInHex[7];
  char symbol[7];
  char hexLoc[7];
  int cnt = 0;
  FILE *fp = fopen(file, "r");

  //read H record
  if (getCertainLine(lineStr, 80, fp, 'H')==0)
    return;
  
  //store control section name
  for (int i = 0; i < 6; i++) {
    if (lineStr[i+1] == ' ') {
      table[id][cnt++][i] = '\0';
      break;
    }
    table[id][cnt][i] = lineStr[i+1];
  }

  //set control section address
  if (id==0)
    estab[id].csaddr = progaddr;
  else
    estab[id].csaddr = estab[id-1].csaddr + estab[id-1].length;

  //set control section length
  for (int i = 0; i < 6; i++)
    lenInHex[i] = lineStr[i+13];
  lenInHex[6] = '\0';
  estab[id].length = htoi(lenInHex);

  //read D record
  if (getCertainLine(lineStr, 80, fp, 'D')==0)
    return;

  //set ESTAB
  int len = strlen(lineStr);
  for (int j, i = 1; i < len; i+=12) {
    for (j = 0; j < 6; j++) {//symbol(label)
      if (lineStr[i+j] == ' ') {
        symbol[j] = '\0';
        break;
      }
      symbol[j] = lineStr[i+j];
    }

    strcpy(table[id][cnt++], symbol);

    for (j = 6; j < 12; j++) {//loaction(address)
      hexLoc[j-6] = lineStr[i+j]; 
    }
    hexLoc[6] = '\0';

    //store symbol and location to ESTAB
    inputEstable(symbol, htoi(hexLoc), id);
    
    //debug
    //printf("id: %d, label: %s, location: %s\n", id, symbol, hexLoc);
    //printf("id: %d, csaddr: %d, label: %s, location: %d\n", id, estab[id].csaddr, symbol, getAddrBySymbol(symbol, id));
  }

  fclose(fp);
}

void createReferArray(FILE* fp, int id, char referArray[][7]) {
  char lineStr[90];
  int lineSize = 90;
  char symbol[7];
  char hexLoc[7];

  //read Refer record
  if (getCertainLine(lineStr, lineSize, fp, 'R')==0)
    return;

  //create refernce array
  int len = strlen(lineStr);
  itoh(estab[id].csaddr, hexLoc, 6);
  strcpy(referArray[1], hexLoc); //csaddr
  for (int j, idx = 2, i = 1; i < len; i+=8) {
    for (j = 0; j < 6; j++) { //get symbol
      if (lineStr[j+i+2] == ' ')
        break;
      symbol[j] = lineStr[j+i+2];
    }
    symbol[j] = '\0';

    itoh(getAddrBySymbol(symbol), hexLoc, 6);
    strcpy(referArray[idx++], hexLoc);
  }
}

void loadTextRecord(FILE* fp, int id, char *lineStr, int lineSize){
  char startLoc[7];
  int startAddr;
  
  char byteLen[3];
  int byteLength;

  //read text records
  do {
    if (lineStr[0] == 'E' || lineStr[0] == 'M')
      break;
    if (lineStr[0] != 'T')
      continue;
    
    for (int i = 0; i < 6; i++)
      startLoc[i] = lineStr[i+1];
    startLoc[6] = '\0';
    startAddr = htoi(startLoc) + estab[id].csaddr;

    for (int i = 0; i < 2; i++)
      byteLen[i] = lineStr[i+7];
    byteLen[2] = '\0';
    byteLength = htoi(byteLen);

    //debug
    //printf("id: %d, startLoc: %s, len: %s\n", id, startLoc, byteLen);

    for (int i = 0; i < byteLength; i++) {
      int loc = startAddr + i;
      int idxOfLineStr = 2*i + 9;

      byteLen[0] = lineStr[idxOfLineStr]; //recycle byteLen array
      byteLen[1] = lineStr[idxOfLineStr+1];
      
      int data = htoi(byteLen);
      //degbug
      //printf("%s(%d)[%d]", byteLen,loc,data);
      mem[loc] = data; //load to memory
    }
    //printf("\n");
  } while(getLine(lineStr, lineSize, fp));
}

void modifyMem(FILE* fp, int id, char referArray[][7], char *lineStr, int lineSize) {
  char hexLoc[10];
  int loc;
  
  char halfByteLen[3];
  int hBlen;

  char idxStr[3];
  int referIdx;

  //read Modify record
  do {
    if (lineStr[0] == 'E')
      break;
    if (lineStr[0] != 'M')
      continue;

    //start location of data which shoud be modified
    for (int i = 0; i < 6; i++)
      hexLoc[i] = lineStr[i+1];
    hexLoc[6] = '\0';
    loc = htoi(hexLoc) + estab[id].csaddr;

    //half bytes
    for (int i = 0; i < 2; i++)
      halfByteLen[i] = lineStr[i+7];
    halfByteLen[2] = '\0';
    hBlen = htoi(halfByteLen);

    int bytes = (hBlen-1)/2+1;

    //reference symbol's idx
    for (int i = 0; i < 2; i++)
      idxStr[i] = lineStr[i+10];
    referIdx = htoi(idxStr);

    //modify
    int data = 0;
    for (int j = 0, i = bytes-1; i >= 0; i--, j+=8) {
      data += ((int)mem[loc+i] & 255) << j; //mem[] is signed char : "FF" -> "FFFFFFFF", so "FFFFFFFF"&255=="000000FF"
    }

    int temp = 0;
    if (hBlen%2) {
      temp = data & ~((1<<4*hBlen)-1);
      data -= temp;
    }

    if (lineStr[9] == '+')
      data += htoi(referArray[referIdx]);
    else
      data -= htoi(referArray[referIdx]);

    data &= (1<<4*hBlen)-1;
    data += temp;

    itoh(data, hexLoc, 2*bytes);
    for (int i = 0; i < bytes; i++) {
      halfByteLen[0] = hexLoc[2*i];
      halfByteLen[1] = hexLoc[2*i+1];
      mem[loc+i] = htoi(halfByteLen);
    }

  } while(getLine(lineStr, lineSize, fp));
}

void load(char *file, int id) {
  FILE* fp = fopen(file, "r");
  char referArray[14][7];
  char lineStr[90] = {0,};
  int lineSize = 90;

  for (int i = 0; i < 14; i++)
    referArray[i][0] = '\0';

  //create reference symbol array
  createReferArray(fp, id, referArray);
  
  //load codes from text record to memory
  loadTextRecord(fp, id, lineStr, lineSize);

  //modify memory data which made of reference symbols or format 4
  modifyMem(fp, id, referArray, lineStr, lineSize);

  fclose(fp);
}

void printEstab(char table[3][7][30]) {
  char hexLoc[5];
  char hexLen[5];
  int totLen = 0;
  printf("control symbol address length\n");
  printf("section name\n");
  printf("---------------------------------\n");
  for (int i = 0; i < input.arg_cnt; i++) {
    itoh(estab[i].csaddr, hexLoc, 4);
    itoh(estab[i].length, hexLen, 4);
    printf("%6s    %6s    %4s    %4s\n", table[i][0], "", hexLoc, hexLen);
    for (int j = 1; j < 7; j++) {
      if (table[i][j][0] == '\0')
        break;
      
      itoh(getAddrBySymbolAndId(table[i][j], i), hexLoc, 4);
      printf("%6s    %6s    %4s    %4s\n", "", table[i][j], hexLoc, "");
    }
    totLen += estab[i].length;
  }
  printf("---------------------------------\n");
  itoh(totLen, hexLen, 4);
  printf("           total length %4s\n", hexLen);
}

void loaderCmd() {
  initEstable();
  initReg();
  initMemory();

  char table[3][7][30];//order of external symbols
  //initialize
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 7; j++) {
      table[i][j][0] = '\0';
    }
  }

  //create external symbol table
  for (int i = 0; i < input.arg_cnt; i++) {
    createEstab(input.args[i], i, table);
    reg[L] += estab[i].length;
  }

  //exceed memory size
  if (estab[input.arg_cnt-1].csaddr+estab[input.arg_cnt-1].length >= MEMSIZE)
    return;

  //load
  for (int i = 0; i < input.arg_cnt; i++) {
    load(input.args[i], i);
  }
  reg[PC] = estab[0].csaddr;

  printEstab(table);
}

void progaddrCmd() {
  progaddr = htoi(input.args[0]);
}