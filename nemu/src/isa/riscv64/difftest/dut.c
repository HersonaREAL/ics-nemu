#include <isa.h>
#include <cpu/difftest.h>
#include <cpu/ifetch.h>
#include <stdint.h>
#include <stdio.h>
#include "../local-include/reg.h"
#include "cpu/decode.h"
#include "macro.h"

#define BUFFSIZE 16

struct ExecInfo {
  vaddr_t pc;
  vaddr_t snpc; // static next pc
  ISADecodeInfo isa;
};

static struct ExecInfo inst_ringbuf[BUFFSIZE];
static size_t start = 0, end = 0;
static char logbuf[128];

static void genInstInfo(struct ExecInfo *inst) {
    memset(logbuf,0,sizeof(logbuf));
    char *p = logbuf;
    int ilen = inst->snpc - inst->pc;
    uint8_t *instr = (uint8_t *)&inst->isa.instr.val;

    p += snprintf(p,sizeof(logbuf),FMT_WORD": ", inst->pc);

    for (int j = 0; j < ilen; j ++) {
      p += snprintf(p,4," %02x", instr[j]);
    }

    p += sprintf(p,"        ");
    #ifdef CONFIG_ITRACE
    void disassemble(char *str,int size, uint64_t pc, uint8_t *code, int nbyte);
    disassemble(p,logbuf + sizeof(logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, inst_ringbuf[i].snpc, inst->pc), (uint8_t *)&inst->isa.instr.val, ilen);
    #endif
}


static void printLeftInst(int n) {
  Decode s;
  struct ExecInfo instInfo;
  s.pc = cpu.pc;
  s.snpc = cpu.pc + 4; //just for riscv
  for (int i = 0; i < n; ++i) {
    s.pc = s.snpc;
    s.isa.instr.val = instr_fetch(&s.snpc, 4);
    
    instInfo.isa = s.isa;
    instInfo.snpc = s.snpc;
    instInfo.pc = s.pc;

    genInstInfo(&instInfo);
    printf("     %s\n",logbuf);
  }
}


void isa_record_recent_inst(Decode *s) {
  if ((end + 1)%BUFFSIZE == start)
    start = (start + 1)%BUFFSIZE;
  inst_ringbuf[end].isa = s->isa;
  inst_ringbuf[end].pc = s->pc;
  inst_ringbuf[end].snpc = s->snpc;
  end = (end + 1)%BUFFSIZE;
}

void isa_show_recent_inst() {
  //empty
  if (start == end)
    return;

  printf("\033[33m\nrecent instr: \n\033[0m");
  
  for (size_t i = start; i != end; i = (i+1)%BUFFSIZE ) {
    genInstInfo(&inst_ringbuf[i]);

    if ((i + 1)%BUFFSIZE != end)
      printf("     %s\n",logbuf);
    else
      printf(" \033[33m--> \033[32m%s\033[0m\n",logbuf);
  }

  // fetch next n inst,just for riscv
  IFDEF(CONFIG_ISA_riscv64, printLeftInst(BUFFSIZE / 2));
}


bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if (ref_r->pc != cpu.pc)
    return false;
  for (size_t i = 0; i < 32; ++i) {
    //printf("ref reg[%ld]: %lx, dut reg[%ld]: %lx\n",i,ref_r->gpr[i]._64,i,cpu.gpr[i]._64);
    if (ref_r->gpr[i]._64 != cpu.gpr[i]._64)
      return false;
  }
  return true;
}

void isa_difftest_attach() {
}
