#include "debug.h"
#include "rtl-basic.h"
#include "rtl/rtl.h"
def_EHelper(jal) {
  rtl_j(s, id_src1->imm + s->pc);
  *ddest = s->snpc;
}

def_EHelper(jalr) {
  rtl_j(s, *dsrc1 + id_src2->imm );
  *ddest = s->snpc;
}