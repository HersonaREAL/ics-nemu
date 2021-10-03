#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_NULLARY(f) f(inv) f(nemu_trap)
#define INSTR_UNARY(f)
#define INSTR_BINARY(f) f(auipc) f(jal) f(lui)
#define INSTR_TERNARY(f) f(ld) f(sd) f(addi) f(jalr) f(lw) f(addw) f(subw) f(sllw) f(srlw) f(sraw) f(add) f(sub) f(slti) f(sltiu) f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu) f(addiw) f(slliw) f(srliw) f(sraiw) f(slli) f(srli) f(srai) f(sb) f(sh) f(sw) f(sll) f(slt) f(sltu) f(xor) f(srl) f(sra) f(or) f(and) f(xori) f(ori) f(andi) f(lb) f(lh) f(lbu) f(lhu) f(lwu)

def_all_EXEC_ID();
