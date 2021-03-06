#include "../local-include/reg.h"
#include "common.h"
#include "cpu/decode.h"
#include "debug.h"
#include <cpu/ifetch.h>
#include <isa-all-instr.h>
#include <stdint.h>
#include <stdio.h>

def_all_THelper();

static uint32_t get_instr(Decode *s) {
  return s->isa.instr.val;
}

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name) (Decode *s, Operand *op, word_t val, bool flag)

static inline def_DopHelper(i) {
  op->imm = val;
}

static word_t zero_null = 0;
word_t *zeroNull() {
  return &zero_null;
}
static inline def_DopHelper(r) {
  bool is_write = flag;
  op->preg = (is_write && val == 0) ? &zero_null : &gpr(val);
}

static inline def_DHelper(I) {
  decode_op_r(s, id_src1, s->isa.instr.i.rs1, false);
  decode_op_i(s, id_src2, (sword_t)s->isa.instr.i.simm11_0, false);
  decode_op_r(s, id_dest, s->isa.instr.i.rd, true);
}

static inline def_DHelper(U) {
  decode_op_i(s, id_src1, (sword_t)s->isa.instr.u.simm31_12 << 12, true);
  decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}

static inline def_DHelper(S) {
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, false);
  sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
  decode_op_i(s, id_src2, simm, false);
  decode_op_r(s, id_dest, s->isa.instr.s.rs2, false);
}

static inline def_DHelper(J) {
  int32_t ori =  s->isa.instr.u.simm31_12;
  sword_t simm = 
          ((ori << 1)  & 0B100000000000000000000) | //imm[20]
          ((ori << 12) & 0B011111111000000000000) | //imm[19:12]
          ((ori << 3)  & 0B000000000100000000000) | //imm[11]
          ((ori >> 8)  & 0B000000000011111111110) ; //imm[10:1]
  simm |= (simm & 0B100000000000000000000) != 0 ? 0xFFFFFFFFFFF00000 : 0;

  decode_op_i(s, id_src1, simm, true);
  decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}

static inline def_DHelper(B) {
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, false);
  decode_op_r(s, id_src2, s->isa.instr.s.rs2, false);

  int32_t ori_12_10_5 = s->isa.instr.s.simm11_5, ori_4_1_11 = s->isa.instr.s.imm4_0;
  sword_t simm = 
          ((ori_12_10_5 << 1 ) & 0B1000000000000) | //imm[12]
          ((ori_4_1_11  << 11) & 0B0100000000000) | //imm[11]
          ((ori_12_10_5 << 5 ) & 0B0011111100000) | //imm[10:5]
          ((ori_4_1_11       ) & 0B0000000011110);  //imm[4:1]
  simm |= (simm & 0B1000000000000) != 0 ? 0xFFFFFFFFFFFFF000 : 0;
  decode_op_i(s, id_dest, simm, false);
}

static inline def_DHelper(R) {
  decode_op_r(s, id_src1, s->isa.instr.r.rs1, false);
  decode_op_r(s, id_src2, s->isa.instr.r.rs2, false);
  decode_op_r(s, id_dest, s->isa.instr.r.rd, true);
}

def_THelper(load) {
  def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", ld);

  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", lb);
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", lh);
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw);
  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", lbu);
  def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", lhu);

  def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", lwu);

  return EXEC_ID_inv;
}

def_THelper(store) {
  def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", sd);

  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", sb);
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", sh);
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw);
  
  return EXEC_ID_inv;
}

def_THelper(calculate_I) {
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", addi);
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", slti);
  def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", sltiu);

  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", xori);
  def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", ori);
  def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", andi);

  def_INSTR_TAB("000000? ????? ????? 001 ????? ????? ??", slli);
  def_INSTR_TAB("000000? ????? ????? 101 ????? ????? ??", srli);
  def_INSTR_TAB("010000? ????? ????? 101 ????? ????? ??", srai);

  return EXEC_ID_inv;
}

def_THelper(calculate_R) {
  def_INSTR_TAB("0000000 ????? ????? 000 ????? ????? ??", add);
  def_INSTR_TAB("0100000 ????? ????? 000 ????? ????? ??", sub);

  def_INSTR_TAB("0000000 ????? ????? 001 ????? ????? ??", sll);
  def_INSTR_TAB("0000000 ????? ????? 010 ????? ????? ??", slt);
  def_INSTR_TAB("0000000 ????? ????? 011 ????? ????? ??", sltu);

  def_INSTR_TAB("0000000 ????? ????? 100 ????? ????? ??", xor);
  def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srl);
  def_INSTR_TAB("0100000 ????? ????? 101 ????? ????? ??", sra);
  def_INSTR_TAB("0000000 ????? ????? 110 ????? ????? ??", or);
  def_INSTR_TAB("0000000 ????? ????? 111 ????? ????? ??", and);

  //rv32m
  def_INSTR_TAB("0000001 ????? ????? 000 ????? ????? ??", mul);
  def_INSTR_TAB("0000001 ????? ????? 100 ????? ????? ??", div);
  def_INSTR_TAB("0000001 ????? ????? 101 ????? ????? ??", divu);
  def_INSTR_TAB("0000001 ????? ????? 110 ????? ????? ??", rem);
  def_INSTR_TAB("0000001 ????? ????? 111 ????? ????? ??", remu);

  return EXEC_ID_inv;
}

def_THelper(calculate_R64) {
  def_INSTR_TAB("0000000 ????? ????? 000 ????? ????? ??", addw);
  def_INSTR_TAB("0100000 ????? ????? 000 ????? ????? ??", subw);
  def_INSTR_TAB("0000000 ????? ????? 001 ????? ????? ??", sllw);
  def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srlw);
  def_INSTR_TAB("0100000 ????? ????? 101 ????? ????? ??", sraw);

  //rv64m
  def_INSTR_TAB("0000001 ????? ????? 000 ????? ????? ??", mulw);
  def_INSTR_TAB("0000001 ????? ????? 100 ????? ????? ??", divw);
  def_INSTR_TAB("0000001 ????? ????? 101 ????? ????? ??", divuw);
  def_INSTR_TAB("0000001 ????? ????? 110 ????? ????? ??", remw);
  def_INSTR_TAB("0000001 ????? ????? 111 ????? ????? ??", remuw);
  return EXEC_ID_inv;
}

def_THelper(branch) {
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", beq);
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", bne);
  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", blt);
  def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", bge);
  def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", bltu);
  def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", bgeu);

  return EXEC_ID_inv;
}

def_THelper(calculate_I64) {
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", addiw);
  def_INSTR_TAB("0000000 ????? ????? 001 ????? ????? ??", slliw);
  def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srliw);
  def_INSTR_TAB("0100000 ????? ????? 101 ????? ????? ??", sraiw);

  return EXEC_ID_inv;
}

def_THelper(csr_I) {
  def_INSTR_TAB("0000000 00000 00000 000 00000 ????? ??", ecall);
  def_INSTR_TAB("0011000 00010 00000 000 00000 ????? ??", mret);
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", csrrw);
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", csrrs);
  return EXEC_ID_inv;
}

def_THelper(main) {
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11001 11", I     , jalr);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00000 11", I     , load);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00100 11", I     , calculate_I);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00110 11", I     , calculate_I64);

  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01000 11", S     , store);

  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00101 11", U     , auipc);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01101 11", U     , lui);

  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11011 11", J     , jal);

  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01100 11", R     , calculate_R);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01110 11", R     , calculate_R64);

  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11000 11", B     , branch);

  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11100 11", I     , csr_I);

  def_INSTR_TAB  ("??????? ????? ????? ??? ????? 11010 11",         nemu_trap);
  return table_inv(s);
};

int isa_fetch_decode(Decode *s) {
  s->isa.instr.val = instr_fetch(&s->snpc, 4);
  int idx = table_main(s);
  return idx;
}
