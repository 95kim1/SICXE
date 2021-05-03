#include "run.h"

int startAddr;
int endAddr;

//get format of mnemonic
int getFormat(int mnemonic) {
  switch(mnemonic) {
    //format 1
    case FIX:
    case FLOAT:
    case HIO:
    case NORM:
    case SIO:
    case TIO:
      return FORMAT1;
    //format 2
    case ADDR:
    case CLEAR:
    case COMPR:
    case DIVR:
    case MULR:
    case RMO:
    case SHIFTL:
    case SHIFTR:
    case SUBR:
    case SVC:
    case TIXR:
      return FORMAT2;
    //format 3/4
    case ADD:
    case ADDF:
    case AND:
    case COMP:
    case COMPF:
    case DIV:
    case DIVF:
    case J:
    case JEQ:
    case JGT:
    case JLT:
    case JSUB:
    case LDA:
    case LDB:
    case LDCH:
    case LDF:
    case LDL:
    case LDS:
    case LDT:
    case LDX:
    case LPS:
    case MUL:
    case MULF:
    case OR:
    case RD:
    case RSUB:
    case SSK:
    case STA:
    case STB:
    case STCH:
    case STF:
    case STI:
    case STL:
    case STS:
    case STSW:
    case STT:
    case STX:
    case SUB:
    case SUBF:
    case TD:
    case TIX:
    case WD:
      return FORMAT3;
  }
  return FORMAT_SIC;
}

//parse object code
void parseObjCode(int pc, struct OBJ_CODE* obj) {
  int addr;
  bool XFlag;
  
  //mnemonic and address mode
  obj->mnemonic = ((int)mem[pc] & 0xFC);
  obj->addrMode = ((int)mem[pc] & 0x03);
  
  //format
  obj->format = getFormat(obj->mnemonic);
  if (obj->format == FORMAT3 && mem[pc+1] & 0x10)
    obj->format = FORMAT4;
  if (obj->format != FORMAT1 && obj->format != FORMAT2 && obj->addrMode == SIC)
    obj->format = FORMAT_SIC;

  //indexing?
  if (obj->format >= FORMAT3)
    XFlag = (mem[pc+1] & 0x80)>>7; 
  else
    XFlag = 0;

  //TA or REG
  if (obj->format == FORMAT2) {
    obj->operand.reg[0] = (mem[pc+1]&0xF0)>>4;
    obj->operand.reg[1] = (mem[pc+1]&0x0F);
  } else if (obj->format >= FORMAT3) {
    //format 3
    addr = mem[pc+2]&0xFF; 
    addr += (mem[pc+1]&0x0F)<<8;

    //negative number(addr)
    if (addr & 0x800)
      addr |= 0xFFFFF000;

    if (obj->format == FORMAT4)//format 4
      addr = ((addr&0x0FFF)<<8) + (mem[pc+3]&0xFF);

    switch((mem[pc+1]&0x60)>>5) {//bp
      case 0:
        obj->operand.TA = addr;
        break;
      case 1:
        obj->operand.TA = addr + pc + ((obj->format == FORMAT3) ? 3 : 4);
        break;
      case 2:
        obj->operand.TA = addr + reg[B];
        break;
    }

    //index
    obj->operand.TA += (XFlag)?reg[X]:0;
  } else { //SIC
    obj->operand.TA = ((mem[pc+1]&0x7F)<<8) + (mem[pc+2]&0xFF);
    
    //index
    obj->operand.TA += (XFlag)?reg[X]:0;
  }
}

void setCC(int cmp) {
  reg[SW] = (cmp & 0x800) ? LT : (cmp == 0) ? EQ : GT;
}

void setF(long long res) {
  reg[F] = res >> 24;
  reg[F2] = res & 0xFFFFFF;
}

long long getF() {
  return (reg[F] << 24) + reg[F2];
}


long long getValue(int TA, int cnt) {
  long long val = 0;
  for (int j = 0, i = cnt-1; i >= 0; i--, j+=8)
    val += mem[TA+i] << j;
  return val & ((1<<(cnt*8)) - 1);
}

void setValue(int TA, int cnt, int rnum) {
  long long m = 0;
  
  if (rnum == F) {
    m = getF();
  } else {
    m = reg[rnum];
    m &= 0xFFFFFF;
  }

  for (int i = cnt-1; i >= 0; i--) {
    mem[TA+i] = m & 0xFF;
    m >>= 8;
  }
}

long long getTargetVal(int addrMode, int TA, int cnt) {
  if (addrMode == IMMEDIATE)
    return TA;
  if (addrMode == INDIRECT)
    return getValue(getValue(TA, cnt), cnt);
  return getValue(TA, cnt);
}

void cmdExecute(struct OBJ_CODE obj) {
  int m;
  long long val;

  int TA = obj.operand.TA;
  if (obj.addrMode == INDIRECT)
    TA = getValue(TA,3);

  switch(obj.mnemonic) {
    //format 1
    case FIX: //A <- (F) [convert to integer]
      reg[A] = getF() & 0xFFFFFF;
      break;
    case FLOAT: //F <- (A) [conver to floating]
      setF(((reg[A]&0x800) ? reg[A]|0xFFF000 : reg[A]) & 0xFFFFFFFFFFFF);
      break;
    case HIO: //Halt I/O channel number (A) : pass
    case NORM: //F <- (F) [normalize float] : pass
    case SIO: //Start I/O channel number (A) : pass
    case TIO: //Test I/O channel number (A) : pass
      break;
    //format 2  
    case ADDR: //r2 <- (r2) + (r1)
      reg[obj.operand.reg[1]] += reg[obj.operand.reg[0]];
      reg[obj.operand.reg[1]] &= 0xFFFFFF;
      break;
    case CLEAR: //r1 <- 0
      reg[obj.operand.reg[0]] = 0;
      break;
    case COMPR: //(r1) : (r2)
      setCC((reg[obj.operand.reg[0]] - reg[obj.operand.reg[1]])&0xFFFFFF);
      break;
    case DIVR: //r2 <- (r2) / (r1)
      reg[obj.operand.reg[1]] /= reg[obj.operand.reg[0]];
      reg[obj.operand.reg[1]] &= 0xFFFFFF;
      break;
    case MULR: //r2 <- (r2) * (r1)
      reg[obj.operand.reg[1]] *= reg[obj.operand.reg[0]];
      reg[obj.operand.reg[1]] &= 0xFFFFFF;
      break;
    case RMO: //r2 <- (r1)
      reg[obj.operand.reg[1]] = reg[obj.operand.reg[0]];
      break;
    case SHIFTL: //r1 <- (r1)  [r1 <- (r1)<<n, r2=n-1]
      reg[obj.operand.reg[0]] <<= reg[obj.operand.reg[1]];
      reg[obj.operand.reg[0]] &= 0xFFFFFF;
      reg[obj.operand.reg[1]]--;    
      break;
    case SHIFTR: //r1 <- (r1)  [r1 <- (r1)>>n, r2=n-1]
      reg[obj.operand.reg[0]] >>= reg[obj.operand.reg[1]];
      reg[obj.operand.reg[0]] &= 0xFFFFFF;
      reg[obj.operand.reg[1]]--;
      break;
    case SUBR: //r2 <- (r2) - (r1)
      reg[obj.operand.reg[1]] -= reg[obj.operand.reg[0]];
      reg[obj.operand.reg[1]] &= 0xFFFFFF;
      break;
    case SVC: //Generate SVC interrupt. [r1=n]
      break;
    case TIXR:  //X <- (X) + 1; (X) : (r1)
      setCC((++reg[X] - reg[obj.operand.reg[0]])&0xFFFFFF);
      break;
    //format 3/4
    case ADD:
      reg[A] += getTargetVal(obj.addrMode, TA, 3);
      break;
    case ADDF:
      setF((getF() + getTargetVal(obj.addrMode, TA, 6)) & 0xFFFFFFFFFFFF);
      break;
    case AND:
      reg[A] &= getTargetVal(obj.addrMode, TA, 3);
      break;
    case COMP:
      setCC((reg[A] - getTargetVal(obj.addrMode, TA, 3))&0xFFFFFF);
      break;
    case COMPF:
      setCC((reg[F] - getTargetVal(obj.addrMode, TA, 6)) & 0xFFFFFFFFFFFF);
      break;
    case DIV:
      val = getTargetVal(obj.addrMode, TA, 3);
      if (val == 0) {
        printf("Error: division by 0\n");
        input.cmd = error;
        return;
      }
      reg[A] /= val;
      reg[A] &= 0xFFFFFF;
      break;
    case DIVF:
      val = getTargetVal(obj.addrMode, TA, 6);
      if (val == 0) {
        printf("Error: division by 0\n");
        input.cmd = error;
        return;
      }
      setF((getF() / val) & 0xFFFFFFFFFFFF);
      break;
    case J:
      reg[PC] = TA;
      break;
    case JEQ:
      if (reg[SW] == EQ)
        reg[PC] = TA;
      break;
    case JGT:
      if (reg[SW] == GT)
        reg[PC] = TA;
      break;
    case JLT:
      if (reg[SW] == LT)
        reg[PC] = TA;
      break;
    case JSUB:
      reg[L] = reg[PC];
      reg[PC] = TA;
      break;
    case LDA:
      reg[A] = getTargetVal(obj.addrMode, TA, 3);
      break;
    case LDB:
      reg[B] = getTargetVal(obj.addrMode, TA, 3);
      break;
    case LDCH:
      reg[A] = getTargetVal(obj.addrMode, TA, 1);
      break;
    case LDF:
      setF(getTargetVal(obj.addrMode, TA, 6));
      break;
    case LDL:
      reg[L] = getTargetVal(obj.addrMode, TA, 3);
      break;
    case LDS:
      reg[S] = getTargetVal(obj.addrMode, TA, 3);
      break;
    case LDT:
      reg[T] = getTargetVal(obj.addrMode, TA, 3);
      break;
    case LDX:
      reg[X] = getTargetVal(obj.addrMode, TA, 3);
      break;
    case LPS://pass
      break;
    case MUL:
      reg[A] = (reg[A] * getTargetVal(obj.addrMode, TA, 3)) & 0xFFFFFF;
      break;
    case MULF:
      setF((getF() * getTargetVal(obj.addrMode, TA, 6)) & 0xFFFFFFFFFFFF);
      break;
    case OR:
      reg[A] |= getTargetVal(obj.addrMode, TA, 3);
      break;
    case RD:
      reg[A] = reg[S]; //to set the result(CC) of COMPR A, S as =
      //setCC(EQ);
      break;
    case RSUB:
      reg[PC] = reg[L];
      break;
    case SSK://pass
      break;
    case STA:
      setValue(TA, 3, A);
      break;
    case STB:
      setValue(TA, 3, B);
      break;
    case STCH:
      mem[TA] = reg[A] & 0xFF;
      break;
    case STF:
      setValue(TA, 6, F);
      break;
    case STI: //pass
      break;
    case STL:
      setValue(TA, 3, L);
      break;
    case STS:
      setValue(TA, 3, S);
      break;
    case STSW:
      setValue(TA, 3, SW);
      break;
    case STT:
      setValue(TA, 3, T);
      break;
    case STX:
      setValue(TA, 3, X);
      break;
    case SUB:
      reg[A] = (reg[A] - getTargetVal(obj.addrMode, TA, 3)) & 0xFFFFFF;
      break;
    case SUBF:
      setF((getF() - getTargetVal(obj.addrMode, TA, 6)) & 0xFFFFFFFFFFFF);
      break;
    case TD:
      setCC(LT);
      break;
    case TIX:
      m = (int)getTargetVal(obj.addrMode, TA, 3);
      reg[X]++;
      setCC((reg[X] < m) ? LT : (reg[X] == m) ? EQ : GT);
      break;
    case WD:
      break;
  }
}

void runCmd() {
  struct OBJ_CODE obj;
  
  while(startAddr <= reg[PC] && reg[PC] < endAddr) {
    //get structure of current object code
    int pc = reg[PC];
    
    //bp print;
    if (inBp(pc)) {
      printReg(pc);
      
      parseObjCode(pc, &obj);
      reg[PC] += (obj.format==FORMAT1) ? 1 : (obj.format==FORMAT2) ? 2 : (obj.format==FORMAT4) ? 4 : 3;
      cmdExecute(obj);
      
      return;
    }

    parseObjCode(pc, &obj);
    reg[PC] += (obj.format==FORMAT1) ? 1 : (obj.format==FORMAT2) ? 2 : (obj.format==FORMAT4) ? 4 : 3;
    cmdExecute(obj);
    
    if (input.cmd == error)
      return;


  }

  printReg(-1);
}