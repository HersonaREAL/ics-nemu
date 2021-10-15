#include "cpu/decode.h"
#include "debug.h"
#include "ftrace.h"
#include "rtl-basic.h"
#include "rtl/rtl.h"
#include <stdio.h>

word_t *zeroNull();

def_EHelper(jal) {
  rtl_j(s, id_src1->imm + s->pc);
  *ddest = s->snpc;

  //ftrace
  #ifdef CONFIG_FTRACE
  if (isFtraceEnable()) {
    if (ddest == &gpr(1)) {
      return printJmpInfo(id_src1->imm + s->pc, true);
    }
  }
  #endif
}

def_EHelper(jalr) {
  rtl_j(s, *dsrc1 + id_src2->imm );
  *ddest = s->snpc;

  //ftrace
  #ifdef CONFIG_FTRACE
  if (isFtraceEnable()) {
    if (ddest == zeroNull() && dsrc1 == &gpr(1) && id_src2->imm == 0) {
      return printJmpInfo(*dsrc1 + id_src2->imm, false);
    }

    if ((ddest == zeroNull()||ddest == &gpr(1)) && dsrc1 == &gpr(6)) {
      return printJmpInfo(*dsrc1 + id_src2->imm, true);
    }
  }
  #endif
}

def_EHelper(beq) {
  rtl_jrelop(s, RELOP_EQ, dsrc1, dsrc2, id_dest->imm + s->pc);
}

def_EHelper(bne) {
  rtl_jrelop(s, RELOP_NE, dsrc1, dsrc2, id_dest->imm + s->pc);
}

def_EHelper(blt) {
  rtl_jrelop(s, RELOP_LT, dsrc1, dsrc2, id_dest->imm + s->pc);
}

def_EHelper(bge) {
  rtl_jrelop(s, RELOP_GE, dsrc1, dsrc2, id_dest->imm + s->pc);
}

def_EHelper(bltu) {
  rtl_jrelop(s, RELOP_LTU, dsrc1, dsrc2, id_dest->imm + s->pc);
}

def_EHelper(bgeu) {
  rtl_jrelop(s, RELOP_GEU, dsrc1, dsrc2, id_dest->imm + s->pc);
}
