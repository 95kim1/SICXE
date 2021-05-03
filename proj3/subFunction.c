#include "subFunction.h"

void itoh(int loc, char *hex, int size) {
  int i = 0;
  char c;
  if (loc==0) {
    for (i = 0; i < size; i++)
      hex[i] = '0';
    hex[i] = '\0';
    return;
  }
  while(loc > 0) {
    c = loc%16;
    if (c > 9)
      c += 55;
    else
      c += '0';
    hex[i++] = c;
    loc /= 16;
  }
  hex[i] = '\0';
  
  for (;i<size;i++) {
    hex[i] = '0';
  }
  hex[i] = '\0';

  int s = 0;
  int e = i-1;
  while(s < e) {
    c = hex[s];
    hex[s] = hex[e];
    hex[e] = c;
    s++; e--;
  }

}

int htoi(char *hex) {
  int num = 0;
  int b = 1;
  int len = strlen(hex);
  for (int i = len - 1; i >= 0; i--) {
    if ('0' <= hex[i] && hex[i] <= '9')
      num += b*(hex[i]-'0');
    else if ('A' <= hex[i] && hex[i] <= 'Z')
      num += b*(hex[i]-55);
    else if ('a' <= hex[i] && hex[i] <= 'z')
      num += b*(hex[i]-87);
    else
      return -1;
    b*=16;
  }
  return num;
}

int dtoi(char *dec) {
  int num = 0;
  int b = 1;
  int len = strlen(dec);
  for (int i = len - 1; i >= 0; i--) {
    if ('0' <= dec[i] && dec[i] <= '9')
      num += b*(dec[i]-'0');
    else
      return -1;
    b*=10;
  }
  return num;
}