#ifndef LOAD_H
#define LOAD_H

#include "estab.h"
#include "subFunction.h"
#include "20141196.h"
#include "assemble.h"
#include "memory.h"
#include "run.h"

#include <string.h>
#include <stdio.h>

extern int progaddr;

void progaddrCmd();

void loaderCmd();

//load obj file to memory
//file: filename, id: id-th file
void laod(char *file, int id);

//print External symbol table
//table: indicate order of external symbols
//table[id][cnt]: id-th obj file's cnt-th symbol
void printEstab(char table[][7][30]);

//modify data in memory with modify record codes in obj file
//fp: (obj)file pointer, id: id-th file, referArray: locations of (other files')external symbols used in this file
//lineStr: one line of an obj file, lineSize: array size of lineStr
void modifyMem(FILE* fp, int id, char referArray[][7], char* lineStr, int lineSize);

// load data to memory with Text record codes
//fp: (obj)file pointer, id: id-th file
//lineStr: one line of an obj file, lineSize: array size of lineStr
void loadTextRecord(FILE* fp, int id, char *lineStr, int lineSize);

// create reference(of external symbols in other files) table  
//fp: (obj)file pointer, id: id-th file, referArray: locations of (other files')external symbols used in this file
//lineStr: one line of an obj file, lineSize: array size of lineStr
void createReferArray(FILE* fp, int id, char referArray[][7], char *lineStr, int lineSize);

// store external symbols to ESTAB
//file: filename, id: id-th file, table: indicate order of external symbols
void createEstab(char *file, int id, char table[][7][30]);

//get certain line of an obj file with a character, C.
//lineStr: one line of an obj file,  size: array size of lineStr,
//fp: (obj)file pointer, C: separater (to determine to get which line of an obj file)
int getCertainLine(char* lineStr, int size, FILE* fp, char C);


#endif