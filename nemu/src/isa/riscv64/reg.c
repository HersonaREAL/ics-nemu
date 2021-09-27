#include <isa.h>
#include <stdio.h>
#include <string.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  int reg_cnt = sizeof(regs) / sizeof(char *);
  for (int i = 0; i < reg_cnt; ++i) {
    uint64_t reg_val = gpr(i);
    if (i%2 == 0 ) {
      printf("\033[31m%s\033[0m: 0x%lx, %ld\t",regs[i], reg_val,reg_val);
    } else {
      printf("\033[31m%s\033[0m: 0x%lx, %ld\n",regs[i],reg_val,reg_val);
    }
  }
  printf("\033[31mpc\033[0m: 0x%lx\n",cpu.pc);
}

word_t isa_reg_str2val(const char *s, bool *success) {
  if (strcmp(s,"$pc") == 0) {
    *success = true;
    return cpu.pc;
  }

  if (strcmp(s, regs[0]) == 0) {
    *success = true;
    return gpr(0);
  }

  int reg_cnt = sizeof(regs) / sizeof(char *);
  for (int i = 1; i < reg_cnt; ++i) {
    if (strcmp(s+1, regs[i]) == 0) {
      *success = true;
      return gpr(i);
    }
  }

  *success = false;
  return -1;
}
