#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "assemble.h"
#include "20141196.h"
#include "opcode.h"
#include "subFunction.h"
#include "hash.h"
#include "symbol.h"
#include "modifyLoc.h"

int errorCheck = 0;     //error to make list, obj files (used at error checking)
int pc = 0;             // Program counter (used at pc relative)
int baseLoc = -1;       // BASE location (used at base relative)
int startLoc = 0;       // start location (used at starting object code)
int lengthLoc = 0;      // location length (used at starting object code)
int maxLengthSym = 0;   // symbol maxLength (used at pass2 for align)
int maxLengthOper = 0;  // operation maxLength (operation1 length + operation2 length; used at pass2 for align)
char commentLine[256];  // record comment
const char INTERMEDIATE_FILE[] = "intermediateFile.txt";

char reg[10][3] = {
  {'A','\0','\0'}, {'X','\0','\0'}, {'L','\0','\0'}, 
  {'B','\0','\0'}, {'S','\0','\0'}, {'T','\0','\0'}, {'F','\0','\0'},
  {'P','C','\0'}, {'S', 'W', '\0'}
};

// Is an asm file line comment?
int isComment(char *lineString) {
  int len = strlen(lineString);
  for (int i = 0; i < len; i++) {
    if (isspace(lineString[i]))
      continue;
    if (lineString[i]=='.')
      return 1;//yes
  }
  return 0;//no
}

void checkTypes(char tokens[][30], int size, int typeNums[]) {
  for (int i = 0; i < size; i++) {
    if (strcmp(tokens[i], "START")==0) {
      typeNums[i] = START_;
      continue;
    }
    else if (strcmp(tokens[i], "END")==0) {
      typeNums[i] = END_;
      continue;
    }
    else if (strcmp(tokens[i], "BASE")==0) {
      typeNums[i] = BASE_;
      continue;
    }
    else if (strcmp(tokens[i], "BYTE")==0) {
      typeNums[i] = BYTE_;
      continue;
    }
    else if (strcmp(tokens[i], "WORD")==0) {
      typeNums[i] = WORD_;
      continue;
    }
    else if (strcmp(tokens[i], "RESB")==0) {
      typeNums[i] = RESB_;
      continue;
    }
    else if (strcmp(tokens[i], "RESW")==0) {
      typeNums[i] = RESW_;
      continue;
    }
    
    // MNEMONIC CHECK
    int s = 0, len = strlen(tokens[i]);
    char tempString[30];
    if (tokens[i][0]=='+')
      s = 1;
    for (int j = s; j <= len; j++)
      tempString[j-s] = tokens[i][j];

    if (getOpcode(tempString, NULL)) {
      typeNums[i] = MNEMO_;
      continue;
    }
    
    // the rest possibility is the SYMBOL
    typeNums[i] = SYMBOL_;
  }
}

// tokenize lineString
// reutrn cnt (of tokenized strings)
int tokenize(char *lineString, char tokens[][30]) {
  int cnt = 0;
  char sep[] = " ,\t"; //delimeter
  char *tempString = strtok(lineString, sep);
  while(tempString) {
    strcpy(tokens[cnt++], tempString);
    tempString = strtok(NULL, sep);
  }
  return cnt;
}

//mnemonic 종류에 따라 Error check 추가 가능
// error: return 1, no error: return 0;
int isErrorLine(int typeNums[], int size) {
  int i;
  int mnemonic = 0;
  for (i = 0; i < size; i++) {
    if (typeNums[i] > SYMBOL_) {
      mnemonic = 1;
      break;
    }
  }
  //i : mnemonic index

  if (!mnemonic || i > 1)
    return 1;

  return 0;
}

// parse one line of asm file
// return count of words (But, error -> return -1)
int parseLine(char *lineString, char tokens[][30], int typeNums[]) {
  //comment check
  if (isComment(lineString)) {
    typeNums[0] = -2;//comment type
    strcpy(commentLine, lineString);
    return 0;
  }

  //후에 ,가 있는지, 어느 위치에 있었는지 체크 필요할 시 추가
  //return -1;

  //tokenize  "label mnemonic operand1, operand2" -> "label" "mnemonic" "operand1" "operand2"
  int size = tokenize(lineString, tokens);

  //check type of tokens
  checkTypes(tokens, size, typeNums);

  //error check
  if (isErrorLine(typeNums, size))
    size = -1;

  return size;
}

// read file
// error: return NULL
char* getLine(char *lineString, int size, FILE* fp) {
  char *tempString =  fgets(lineString, size, fp);
  int len = strlen(lineString);
  if (lineString[len-1]=='\n')
    lineString[len-1] = '\0';
  return tempString;
}

// return a type of line (START, END, MNEMONIC, BASE, BYTE, ...)
int whichType(int typeNums[], int size, int *idx) {
  if (typeNums[0] == -2) {
    return COMMENT_;
  }
  int i;
  for (i = 0; i < size; i++) {
    if (typeNums[i] >= START_) {
      *idx = i;
      break;
    }
  }
  return typeNums[i];
}

void assemblePass1() {
  FILE *fp = fopen(input.args[0], "r");
  FILE *fp2 = fopen(INTERMEDIATE_FILE, "w");

  char lineString[256]; // a line of .asm file
  char tokens[5][30]; // lineString is tokenized into tokens array
  int tokenCnt = 0; // "label mnemonic operand1 operand2" -> "label" "mnemonic" "operand1" "operand2"
  int typeNums[5]; //each type of tokens[0~(tokensCnt-1)]
  int line = 5; // line number
  int loc = 0; //location counter
  int format = 0; //next loc = cur lor + format
  int END_DIREC = 0; //check END 

  while(!END_DIREC && getLine(lineString, 256, fp)) {
    tokenCnt = parseLine(lineString, tokens, typeNums);
    
    if (tokenCnt==-1) {
      printf("Error: line %d\n", line);
      errorCheck = 1;
      return;
    }

    int prev_loc = loc;
    int idx, temp = 0; //idx: mnemonic(direcvite) index (mnemonic(directive) == tokens[idx])
    int type = whichType(typeNums, tokenCnt, &idx);
    switch(type) { //which type of this line? (START, END, DIRECTIVE, MNEMONIC,...)
      case START_: { //starting location counter setting
        if (idx < tokenCnt)
          temp = htoi(tokens[idx+1]);
        
        if (temp == -1) {
          printf("Error: line %d\n", line);
          errorCheck = 1;
          return;
        }
        
        loc = temp;
        startLoc = temp;
        
        if (idx+1 == tokenCnt) { //No LOCATION next to "START" in asm file line
          strcpy(tokens[idx+1], ".");
        }

        if (idx > 0)
          maxLengthSym = strlen(tokens[idx-1]); 
        break;
      }
      case END_: { //END of asm file
        END_DIREC = 1;
        lengthLoc = (loc - format - startLoc + 1);
        if (idx+1 == tokenCnt) { //No LABEL next to "END" in asm file line
          strcpy(tokens[idx+1],".");
        }
        break;
      }
      case MNEMO_: {
        //check format 4 (+JEQ or JEQ)
        if (tokens[idx][0] == '+') {
          loc += 4;
          format = 4;
          break;
        }
        struct hash_node* cur = getOpcode(tokens[idx], NULL);
        loc += cur->format[0] - '0';
        format = loc - prev_loc;
        break;
      }
      case BASE_: {
        break;
      }
      case BYTE_: {
        // ex) C'EOF'-> 3Bytes
        int len = strlen(tokens[idx+1]);
        if ((tokens[idx+1][0] == 'C' || tokens[idx+1][0] == 'X') && !(tokens[idx+1][1] == '\'' && tokens[idx+1][len-1] == '\'')) {
          printf("Error: line %d\n", line);
          errorCheck = 1;
          return;
        }
        if (tokens[idx+1][0] == 'X') {
          if ((len-3) % 2 != 0) {
            printf("Error: line %d\n", line);
            errorCheck = 1;
            return;
          }
          len = (len-3)/2 + 3;
        }

        loc += len - 3;
        format = loc - prev_loc;
        break;
      }
      case WORD_: {
        int len = strlen(tokens[idx+1]);
        for (int i = 0; i < len; i++) {
          char c = tokens[idx+1][i];
          if (!('0' <= c && c <= '9')) {
            printf("Error: line %d\n", line);
            errorCheck = 1;
            return;
          }
        }
        loc += 3;
        format = 3;
        break;
      }
      case RESB_: {
        temp = dtoi(tokens[idx+1]);
        if (temp == -1) {
          printf("Error: line %d\n", line);
          errorCheck = 1;
          return;
        }
        loc += temp;
        format = loc - prev_loc;
        break;
      }
      case RESW_: {
        temp = dtoi(tokens[idx+1]);
        if (temp == -1) {
          printf("Error: line %d\n", line);
          errorCheck = 1;
          return;
        }
        loc += 3 * temp;
        format = loc - prev_loc;
        break;
      }
    }

    //update max length of operation1+2
    if (idx+1 < tokenCnt) {
      int tempLength = strlen(tokens[idx+1]);
      if (idx+2 < tokenCnt)
        tempLength += strlen(tokens[idx+2]);
      if (maxLengthOper < tempLength)
        maxLengthOper = tempLength;
    }

    char tempLabel[30] = ".\0";

    //update SYMBOL TABLE, if label is in this line.
    if (type != COMMENT_ && type != START_ && type != END_ && idx > 0) {
      if (!pushSymtab(tokens[idx-1], prev_loc)) {
        errorCheck = 1;
        printf("Error: line %d\n", line);
        return;
      }
      strcpy(tempLabel, tokens[idx-1]);
      if (strlen(tempLabel) > maxLengthSym)
        maxLengthSym = strlen(tempLabel);
    }

    //update intermediate file
    //line loc label type mnemonic(directive) format operand1 operand2
    if (tokenCnt == 0) //comment line
      fprintf(fp2, "%d %d %d %s\n", line, -2, type, commentLine);
    else if (type == START_ || type == END_ || type == BASE_) {//START line or END line or BASE line
      if (idx==0)
        fprintf(fp2, "%d %d %d %s %s %d %s .\n", line, prev_loc, type, ".", tokens[idx], 0, tokens[idx+1]);
      else
        fprintf(fp2, "%d %d %d %s %s %d %s .\n", line, prev_loc, type, tokens[idx-1], tokens[idx], 0, tokens[idx+1]);
    }
    else if (tokenCnt - idx == 1)
      fprintf(fp2, "%d %d %d %s %s %d . .\n", line, prev_loc, type, tempLabel, tokens[idx], format);
    else if (tokenCnt - idx  < 3)
      fprintf(fp2, "%d %d %d %s %s %d %s .\n", line, prev_loc, type, tempLabel, tokens[idx], format, tokens[idx+1]);
    else if (tokenCnt - idx == 3)
      fprintf(fp2, "%d %d %d %s %s %d %s %s\n", line, prev_loc, type, tempLabel, tokens[idx], format, tokens[idx+1], tokens[idx+2]);

    line += 5;
  }  

  fclose(fp);
  fclose(fp2);
}

// copy between two lines of intermediate file
void copyLine(struct intermediateFileLine *dest, struct intermediateFileLine *src) {
  dest->line = src->line;
  dest->loc = src->loc;
  dest->type = src->type;
  if (dest->type == COMMENT_)
    return;
  strcpy(dest->label, src->label);
  strcpy(dest->mnemo, src->mnemo);
  dest->format = src->format;
  strcpy(dest->operand1, src->operand1);
  strcpy(dest->operand2, src->operand2);
}

// read a line from intermediate file
void getLineOfIntermediateFile(struct intermediateFileLine *lineInfo, FILE *fp) {
  fscanf(fp, "%d %d %d", &lineInfo->line, &lineInfo->loc, &lineInfo->type);
  if (lineInfo->type == COMMENT_) {
    fgets(commentLine, 256, fp);
    int len = strlen(commentLine);
    if (commentLine[len-1] == '\n') //\n제거
      commentLine[len-1] = '\0';
    int i = 0;
    while(commentLine[i++] != '.');
    int j = 0; i--;
    while(i <= len) { //remove charaters before the first '.'
      commentLine[j++] = commentLine[i++];
    }
    return;
  }
  fscanf(fp, "%s %s %d %s %s", lineInfo->label, lineInfo->mnemo, &lineInfo->format, lineInfo->operand1, lineInfo->operand2);
}

// remove extension from filename
void getPureFileName(char *filename, char *fullname) {
  strcpy(filename, fullname);
  int len = strlen(filename);
  int i;
  for (i = len; i >= 0; i--) {
    if (filename[i] == '.')
      break;
  }
  filename[i] = '\0';
}

// write starting line of .obj file
void writeStartingOfObjFile(FILE *fp, char *programName) {
  if (programName[0] == '.') {
    programName[0] = '\0';
  }
  
  //write program name
  fprintf(fp, "H%s", programName);
  
  int len = strlen(programName);
  int blank = (6 - len % 6) % 6;
  char blankString[7];
  int i;
  for (i = 0; i < blank; i++)
    blankString[i] = ' ';
  blankString[i] = '\0';
  //fill blanks
  fprintf(fp, "%s", blankString);

  //starting location, length of code(location)
  char startHex[7], lengthHex[7];
  itoh(startLoc, startHex, 6);
  itoh(lengthLoc, lengthHex, 6);
  fprintf(fp, "%s%s\n", startHex, lengthHex);
}

//copy: from src to dest (without a character, src[0])
void removeFrontChar(char *dest, char *src) {
  int len = strlen(src);
  for (int i = 0; i < len; i++) {
    dest[i] = src[i+1];
  }
}

// register string to number
int whichReg(char *regStr) {
  if (strcmp(regStr, "A")==0) {
    return A;
  } else if (strcmp(regStr, "X")==0) {
    return X;
  } else if (strcmp(regStr, "L")==0) {
    return L;
  } else if (strcmp(regStr, "B")==0) {
    return B;
  } else if (strcmp(regStr, "S")==0) {
    return S;
  } else if (strcmp(regStr, "T")==0) {
    return T;
  } else if (strcmp(regStr, "F")==0) {
    return F;
  } else if (strcmp(regStr, "PC")==0) {
    return PC;
  } else if (strcmp(regStr, "SW")==0) {
    return SW;
  }
  int temp = dtoi(regStr);
  if (temp == -1) return -1;
  return temp;
}

// make object code and return object code's length (in byte unit)
int makeObjectCodeForMnemonic(struct intermediateFileLine *curLine, char*objCode) {
  int temp;
  // get format
  int format = curLine->format;
  
  // get opcode
  char mnemonic[7];
  char opcode[3];
  if (curLine->mnemo[0] == '+')
    removeFrontChar(mnemonic, curLine->mnemo);
  else
    strcpy(mnemonic, curLine->mnemo);
  getOpcode(mnemonic, opcode);
  
  //format 1 objCode
  if (format == 1) {
    strcpy(objCode, opcode);
    return 1;
  }

  //format 2 objCode
  if (format == 2) {
    //copy opcode
    strcpy(objCode, opcode);

    char regChar[2] = " \0";
    //copy register number
      //operand1
    regChar[0] = (whichReg(curLine->operand1)+'0');
    strcat(objCode, regChar);
      //opernad2
    if (curLine->operand2[0] == '.') {
      objCode[3] = '0';
      objCode[4] = '\0';
    } else {
      temp = whichReg(curLine->operand2);
      if (temp<10)
        regChar[0] = (temp+'0');
      else
        regChar[0] = (temp+55); //SHIFT r1, n
      strcat(objCode, regChar);
    }
    return 2;
  }

  //format 3 objCode
  if (format == 3) {
      //opcode+ni
    int temp = htoi(opcode);
    if (curLine->operand1[0] == '#')//immediate access
      temp+=1; //ni : 01 -> 1
    else if (curLine->operand1[0] == '@')//indirect access
      temp+=2; //ni : 10 -> 2
    else//simple access
      temp+=3; //ni : 11 -> 3
    itoh(temp, opcode, 2); //integer -> hex string
    strcpy(objCode, opcode);

    if (curLine->operand1[0] == '.') {//no operand
      strcat(objCode, "0000");
      return 3;
    }

      //address
    int location;
    char operand[30];
    if (curLine->operand1[0] == '#') {//label or constant
      removeFrontChar(operand, curLine->operand1);
      if ('0'<=operand[0]&&operand[0]<='9')//constant
        location = dtoi(operand);
      else//label
        location = findSymtab(operand);
    } 
    else {//label
      if (curLine->operand1[0] == '@')
        removeFrontChar(operand, curLine->operand1);
      else
        strcpy(operand, curLine->operand1);
      location = findSymtab(operand);
    }

      //xbpe
    temp = 0;
    if (curLine->operand2[0] == 'X')
      temp+=8;
    if (!('0'<=operand[0]&&operand[0]<='9')) {//operand is constant -> no pc, base relative
      if (-2048 <= location - pc && location - pc < 2048) {//pc relative
        temp+=2;
        location -= pc;
        location = (location & 0xFFF); // turn the front 20bits of location into zeros
      } else if (baseLoc!=-1 && -2048 <= location-baseLoc && location-baseLoc < 2048) {//base relative
        temp+=4;
        location -= baseLoc;
        location = (location & 0xFFF); // turn the front 20bits of location into zeros
      } else {
        printf("Error: line %d\n", curLine->line);
        errorCheck = 1;
        return 0;
      }
    } 
    itoh(temp, opcode, 1); //reuse opcode for xbpe
    strcat(objCode, opcode);

    //add address to opjCode
    itoh(location, opcode, 3); //reuse opcode for address
    strcat(objCode, opcode);
    return 3;
  }

  //format 4 objCode
    //opcode+ni
  temp = htoi(opcode);
  if (curLine->operand1[0] == '#')//immediate
    temp+=1; //ni : 01 -> 1
  else if (curLine->operand1[0] == '@')//indirect
    temp+=2; //ni : 10 -> 2
  else//simple
    temp+=3; //ni : 11 -> 3
  itoh(temp, opcode, 2); //integer -> hex string
  strcpy(objCode, opcode);

    //xbpe
  temp = 0;
  if (curLine->operand2[0] == 'X')
    temp+=8;
  temp+=1;//because of e
  itoh(temp, opcode, 1); //reuse opcode for xbpe
  strcat(objCode, opcode);

    //address
  char operand[30];
  if (curLine->operand1[0] == '#') {//decimal opearand
    removeFrontChar(operand, curLine->operand1);
    temp = dtoi(operand);
  }
  else {//label
    strcpy(operand, curLine->operand1);
    temp = findSymtab(operand);
  }
  itoh(temp, opcode, 5); //reuse opcode for address
  strcat(objCode, opcode);
  return 4;
}

// record object code line to obj file 
// if objCodeLine is full enough or location is too far,
// else record object code to objCodeLine
void writeObjFileLine(FILE *fpObj, char *objCodeLine, char *objCode, int *startLocObjLine, int curLoc, struct modifyList *modifyList, char isLoc) {
  int diff = curLoc - *startLocObjLine; //location length
  int lengthObjLine = strlen(objCodeLine); //objCodeLine length
  int lengthObjCode = strlen(objCode);

  char writeFlag = 1;

  if (diff > 30 || lengthObjLine + lengthObjCode > 60) {
    // write and empty objCodeLine to .obj
    // ,then write objCode to objCodeLine
    char tempString[7];

    //write T, local staring location for this line
    itoh(*startLocObjLine, tempString, 6);
    fprintf(fpObj, "T%s", tempString);
    
    //local length for this line, objCodeLine
    itoh(lengthObjLine/2, tempString, 2);
    fprintf(fpObj, "%s%s\n", tempString, objCodeLine);
    
    //record objCode and curLocation(as a local staring location)
    strcpy(objCodeLine, objCode);
    lengthObjLine = lengthObjCode;
    *startLocObjLine = curLoc;
  }
  else {
    lengthObjLine += lengthObjCode;
    strcat(objCodeLine, objCode);
    writeFlag = 0;
  }

  //recor format 4's idx to modifyList
  if (lengthObjCode == 8 && isLoc == 'Y') {//obj code is format 4 and address part of it is a location
    pushModifyNode(modifyList, makeModifyNode(modifyList->nextIdx + 1, NULL));
  }

  // To write obj file means that Text record's starting location changed
  //so next modfiying location should be local starting location.
  if (writeFlag)
    modifyList->nextIdx = *startLocObjLine;
  else // no write -> just add (lefnth of object code)/2  (length: half byte, length/2: byte)
    modifyList->nextIdx += lengthObjCode/2;
}

void assemblePass2() {
  char filename[30];
  getPureFileName(filename, input.args[0]);
  FILE *fpInter = fopen(INTERMEDIATE_FILE, "r");
  FILE *fpList = fopen(strcat(filename, ".list"), "w");
  filename[strlen(filename)-5] = '\0';
  FILE *fpObj = fopen(strcat(filename, ".obj"), "w");
  //printf("%p : %s\n", fpObj, filename);

  char objCode[10]; // object code (opcode or opcode r1 r2 or opcode ni xbpe address or constant)
  char objCodeLine[61]; //string of object codes for .obj file line
  int startLocObjLine = 0; //record ObjCodeLine's starting location(address)
  char hexLoc[5]; // location(hex)
  struct intermediateFileLine curLine, nextLine;
  struct modifyList modifyList; //record format 4 object code's indices in obj file
  
  initModifyList(&modifyList);

  objCode[0] = '\0';
  objCodeLine[0] = '\0';

  maxLengthOper += 6;

  // write .list, .obj files
  getLineOfIntermediateFile(&curLine, fpInter);
  while(strcmp(curLine.mnemo, "END") != 0) {
    if (curLine.type == COMMENT_)//comment print to intermediate file
      fprintf(fpList, "%-8d %-8s %s\n", curLine.line, " ", commentLine);
    
    getLineOfIntermediateFile(&nextLine, fpInter);

    pc = nextLine.loc; //program counter

    //change location type from integer(decimal) to hex string
    if (curLine.loc >= 0)
      itoh(curLine.loc, hexLoc, 4);

    //type Check, and write .obj and .list files
    switch(curLine.type) {
      case COMMENT_: {// done before get nextLine from intermediate file
        break;
      }
      case START_: {
        //write .obj file
        //write starting of object file ("H name startLoc lengthLoc")
        writeStartingOfObjFile(fpObj, curLine.label); //curLine.label == "." -> curLine.label = "\0"
        startLocObjLine = startLoc;
        modifyList.nextIdx = startLoc;

        if (curLine.label[0] == '\0') {
          curLine.label[0]=' ';
          curLine.label[1] = '\0';
        }
        
        //write .list file(start)
        int blankCnt = maxLengthSym-strlen(curLine.label);
        fprintf(fpList, "%-8d %-8s %s", curLine.line, hexLoc, curLine.label);
        for (int i = 0; i < blankCnt; i++)
          fprintf(fpList, " ");
        fprintf(fpList, " %-7s %s\n", curLine.mnemo, curLine.operand1);
        //write .list file(end)
        break;
      }
      case END_: { //done after end of [ while(strcmp(curLine.mnemo, "END") != 0) ]
        break;
      }
      case MNEMO_: {
        if (curLine.label[0] == '.') {
          curLine.label[0] = ' ';
          curLine.label[1] = '\0';
        }
        
        //"line label mnemonic operand1, 2" -> objCode("XXXXXX")
        FILE *tempFp = fpObj; // fpObj's value is changed after bellow function, but I don't knwo why... (so store it to temp space)
        makeObjectCodeForMnemonic(&curLine, objCode);
        fpObj = tempFp;

        //write .list file (start)
        int blankCnt = maxLengthSym - strlen(curLine.label);
        fprintf(fpList, "%-8d %-8s %s", curLine.line, hexLoc, curLine.label);
        
        for (int i = 0; i < blankCnt; i++) //blanks after label
          fprintf(fpList, " ");
        
        fprintf(fpList, " %-7s ", curLine.mnemo);
        
        blankCnt = maxLengthOper+2;
        if (curLine.operand1[0] != '.') {
          fprintf(fpList, "%s", curLine.operand1);
          blankCnt -= strlen(curLine.operand1);
        }
        if (curLine.operand2[0] != '.') {
          fprintf(fpList, ", %s", curLine.operand2);
          blankCnt -= strlen(curLine.operand2) + 2;
        }

        for (int i = 0; i < blankCnt; i++) //blanks after operand
            fprintf(fpList, " ");
        fprintf(fpList, " %s\n", objCode);
        //fprintf(fpList, "\n");
        //write .list file (end)

        //is operand1 label or constant
        char isLoc = 'Y';
        if ('0'<=curLine.operand1[1]&&curLine.operand1[1]<='9')
          isLoc = 'N';

        //write .obj file
        writeObjFileLine(fpObj, objCodeLine, objCode, &startLocObjLine, curLine.loc, &modifyList, isLoc);
        break;
      }
      case BASE_: {
        //record loaction for BASE
        baseLoc = findSymtab(curLine.operand1);
        if (curLine.label[0] == '.') {
          curLine.label[0] = ' ';
          curLine.label[1] = '\0';
        }

        //write .list file(start)
        int blankCnt = maxLengthSym - strlen(curLine.label);
        fprintf(fpList, "%-8d %-8s %s", curLine.line, " ", curLine.label);
        for (int i = 0; i < blankCnt; i++)
          fprintf(fpList, " ");
        fprintf(fpList, " %-7s %s\n", curLine.mnemo, curLine.operand1);
        //wirete .list file(end)
        break;
      }
      case BYTE_: {
        if (curLine.label[0] == '.') {
          curLine.label[0] = ' ';
          curLine.label[1] = '\0';
        }
        // operand -> objcode  (ex C'EOF'->454F46, X'F1'->F1)
        switch (curLine.operand1[0]) {
          case 'C': {
            //write .list file
            int blankCnt = maxLengthSym - strlen(curLine.label);
            fprintf(fpList, "%-8d %-8s %s", curLine.line, hexLoc, curLine.label);
            for (int i = 0; i < blankCnt; i++)
              fprintf(fpList, " ");
            fprintf(fpList, " %-7s %s", curLine.mnemo, curLine.operand1);

            blankCnt = maxLengthOper+2 - strlen(curLine.operand1);
            for (int i = 0; i < blankCnt; i++)
              fprintf(fpList, " ");
            fprintf(fpList, " ");
            
            // constant(operand) record (to .list and .obj files)
            int len = strlen(curLine.operand1);
            int tempNum;
            int loc = curLine.loc;
            for (int i = 2; i < len-1; i++) {
              //transfer each character into hexNum and ,then, into String
              tempNum = curLine.operand1[i];
              char hex[3];
              itoh(tempNum, hex, 2);
              
              //write .list file
              fprintf(fpList, "%s", hex);

              //write .obj file
              writeObjFileLine(fpObj, objCodeLine, hex, &startLocObjLine, loc, &modifyList, ' '); 
              loc += 1;
            }
            fprintf(fpList, "\n");//write .list file
            break;
          }
          case 'X': {
            int i = 2;
            while(curLine.operand1[i]!='\'') {
              if (!('0'<=curLine.operand1[i] && curLine.operand1[i]<='9') && !('A'<=curLine.operand1[i] && curLine.operand1[i]<='F') && !('a'<=curLine.operand1[i] && curLine.operand1[i]<='f')) {
                errorCheck = 1;
                printf("Error: line %d\n", curLine.line);
                freeModifyList(&modifyList);
                return;
              }
              objCode[i-2] = curLine.operand1[i];
              i++;
            }
            objCode[i-2] = '\0';

            //write .list file(start)
            int blankCnt = maxLengthSym - strlen(curLine.label);
            fprintf(fpList, "%-8d %-8s %s", curLine.line, hexLoc, curLine.label);
            for (int i = 0; i < blankCnt; i++)
              fprintf(fpList, " ");
            fprintf(fpList, " %-7s %s", curLine.mnemo, curLine.operand1);
            blankCnt = maxLengthOper+2 - strlen(curLine.operand1);
            for (int i = 0; i < blankCnt; i++)
              fprintf(fpList, " ");
            fprintf(fpList, " %s\n", objCode);
            //write .list file(end)
  
            //write .obj file(start)
            writeObjFileLine(fpObj, objCodeLine, objCode, &startLocObjLine, curLine.loc, &modifyList, ' '); 
            break;
          }
        }
        break;
      }
      case WORD_: {
        if (curLine.label[0] == '.') {
          curLine.label[0] = ' ';
          curLine.label[1] = '\0';
        }

        // get rid of '#'; #3456 -> 3456
        char constant[9];
        if (curLine.operand1[0] == '#')
          removeFrontChar(constant, curLine.operand1);
        else
          strcpy(constant, curLine.operand1);
        
        //change decimal string to hex string
        int tempNum = dtoi(constant);
        itoh(tempNum, objCode, 6);

        //write .list file(start)
        int blankCnt = maxLengthSym - strlen(curLine.label);
        fprintf(fpList, "%-8d %-8s %s", curLine.line, hexLoc, curLine.label);
        for (int i = 0; i < blankCnt; i++)
          fprintf(fpList, " ");
        fprintf(fpList, " %-7s %s", curLine.mnemo, curLine.operand1);
        blankCnt = maxLengthOper+2 - strlen(curLine.operand1);
        for (int i = 0; i < blankCnt; i++)
          fprintf(fpList, " ");
        fprintf(fpList, " %s\n", objCode);
        //write .list file(end)

        //write .obj file(start)
        writeObjFileLine(fpObj, objCodeLine, objCode, &startLocObjLine, curLine.loc, &modifyList, ' '); 
        break;
      }
      case RESB_:
      case RESW_: {
        if (curLine.label[0] == '.') {
          curLine.label[0] = ' ';
          curLine.label[1] = '\0';
        }
        
        //write .list file
        int blankCnt = maxLengthSym - strlen(curLine.label);
        fprintf(fpList, "%-8d %-8s %s", curLine.line, hexLoc, curLine.label);
        for (int i = 0; i < blankCnt; i++)
          fprintf(fpList, " ");
        fprintf(fpList, " %-7s %s\n", curLine.mnemo, curLine.operand1);
        break;
      }
    }

    copyLine(&curLine, &nextLine);
  }//END of (write .list .obj files)

  //curLine.mnemonic == "END"
  if (curLine.operand1[0] == '.') {
    curLine.operand1[0] = '\0';
  }

  if (curLine.label[0] == '.') {
    curLine.label[0] = ' ';
    curLine.label[1] = '\0';
  }

  //write .list file(start)
  int blankCnt = maxLengthSym-strlen(curLine.label);
  fprintf(fpList, "%-8d %-8s %s", curLine.line, hexLoc, curLine.label);
  for (int i = 0; i < blankCnt; i++)
    fprintf(fpList, " ");
  fprintf(fpList, " %-7s %s\n", curLine.mnemo, curLine.operand1);
  //write .list file(end)


  //write .obj file
  int lengthObjLine = strlen(objCodeLine);
  itoh(startLocObjLine, objCode, 6);
  fprintf(fpObj, "T%s", objCode);
  itoh(lengthObjLine, objCode, 2);
  fprintf(fpObj, "%s%s\n", objCode, objCodeLine);
  
  //write .obj file (record modify code)
  struct modifyNode *cur = modifyList.head;
  while(cur!=NULL) {
    itoh(cur->idx, objCode, 6);
    fprintf(fpObj, "M%s05\n", objCode);
    cur = cur->next;
  }
  freeModifyList(&modifyList);

  //write .obj file (record END line)
  itoh(startLoc, objCode,6);
  fprintf(fpObj, "E%s\n", objCode);

  fclose(fpInter);
  fclose(fpList);
  fclose(fpObj);
}

void assembleCmd() {
  //filename check
  int len = strlen(input.args[0]);
  if (strcmp(input.args[0]+(len-4), ".asm")!=0) {
    printf("Error: not a \'.asm\' extension file\n");
    input.cmd = error;
    return;
  }

  //SYMBOL TABLE backup
  backupSymtab();

  //pass1 process: create intermediate file
  //create LOCCTR, SYMTAB
  assemblePass1();
  
  //an error occur in process pass1.
  if (errorCheck) {
    //remove(.list, .obj)
    // Recovery SYMBOL TABLE
    recoverySymtab();
    return;
  }
  
  //pass2 process:
  assemblePass2();

  //if create list file successfully, 
  //then free backup symtab;
  if (!errorCheck) {
    freeBackupSymtab();

    //print  [2_5.list], [2_5.obj]
    char filename[30];
    strcpy(filename, input.args[0]);
    len = strlen(filename);
    filename[len-3] = '\0';
    strcat(filename, "list");
    printf("[%s], ", filename);
    filename[len-3] = '\0';
    strcat(filename, "obj");
    printf("[%s]\n", filename);
  }

  //delete intermediate file
  remove(INTERMEDIATE_FILE);
}