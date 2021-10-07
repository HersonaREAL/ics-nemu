#include "cpu/decode.h"
#include "rtl-basic.h"
#include "rtl/rtl.h"
def_EHelper(auipc) {
  rtl_li(s, ddest, id_src1->imm + s->pc);
}

def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(addi) {
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(addiw) {
  rtl_addwi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slliw) {
  rtl_sllwi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srliw) {
  rtl_srlwi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sraiw) {
  rtl_srawi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(addw) {
  rtl_addw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(subw) {
  rtl_subw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sllw) {
  rtl_sllw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srlw) {
  rtl_srlw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sraw) {
  rtl_sraw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(add) {
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub) {
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slti) {
  rtl_setrelopi(s, RELOP_LT, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sltiu) {
  rtl_setrelopi(s, RELOP_LTU, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slli) {
  rtl_slli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srli) {
  rtl_srli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srai) {
  rtl_srai(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sll) {
  rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slt) {
  rtl_setrelop(s, RELOP_LT, ddest, dsrc1, dsrc2);
}

def_EHelper(sltu) {
  rtl_setrelop(s, RELOP_LTU, ddest, dsrc1, dsrc2);
}

def_EHelper(xor) {
  rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srl) {
  rtl_srl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra) {
  rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or) {
  rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and) {
  rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(xori) {
  rtl_xori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(ori) {
  rtl_ori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(andi) {
  rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(mulw) {
  rtl_mulw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divw) {
  rtl_divw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divuw) {
  rtl_divuw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remw) {
  rtl_remw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remuw) {
  rtl_remuw(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mul) {
  rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}