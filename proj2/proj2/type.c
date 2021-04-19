#include "20141196.h"
#include <stdio.h>

void typeCmd() {
  FILE *fp = fopen(input.args[0], "r");
  if (fp == NULL) {
    input.cmd = error;
    printf("Error: there is no file, %s\n", input.args[0]);
    return;
  }
  char str[256];
  while(fgets(str, 256, fp)) {
    printf("%s", str);
  }
}