#include "../local-include/reg.h"
#include "common.h"
#include "cpu/decode.h"
#include "debug.h"
#include <cpu/ifetch.h>
#include <isa-all-instr.h>
#include <stdint.h>

def_all_THelper();

static uint32_t get_instr(Decode *s) {
  return s->isa.instr.val;
}

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name) (Decode *s, Operand *op, word_t val, bool flag)

static inline def_DopHelper(i) {
  op->imm = val;
  print_Dop(op->str, OP_STR_SIZE, (flag ? "0x%lx" : "%ld"), op->imm);
}

static inline def_DopHelper(r) {
  bool is_write = flag;
  static word_t zero_null = 0;
  op->preg = (is_write && val == 0) ? &zero_null : &gpr(val);
  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(val, 4));
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
  simm |= (simm & 0B100000000000000000000) != 0 ? 0xFFFFFFFFFFE00000 : 0;

  decode_op_i(s, id_src1, simm, true);
  decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}

def_THelper(load) {
  def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", ld);
  return EXEC_ID_inv;
}

def_THelper(store) {
  def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", sd);
  return EXEC_ID_inv;
}

def_THelper(calculate) {
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", addi);
  TODO();
  return EXEC_ID_inv;
}

def_THelper(main) {
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11001 11", I     , jalr);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00000 11", I     , load);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00100 11", I     , calculate);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01000 11", S     , store);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00101 11", U     , auipc);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11011 11", J     , jal);
  def_INSTR_TAB  ("??????? ????? ????? ??? ????? 11010 11",         nemu_trap);
  return table_inv(s);
};

int isa_fetch_decode(Decode *s) {
  s->isa.instr.val = instr_fetch(&s->snpc, 4);
  int idx = table_main(s);
  return idx;
}
