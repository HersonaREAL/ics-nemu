#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lw) f(sw) f(inv) f(nemu_trap) f(ld) f(lb) f(lh) f(lbu) f(lhu) f(lwu) f(sd) f(sb) f(sh) f(addi) f(slti) f(sltiu) f(xori) f(ori) f(andi) f(slli) f(srli) f(srai) f(add) f(sub) f(sll) f(slt) f(sltu) f(xor) f(srl) f(sra) f(or) f(and) f(mul) f(addw) f(subw) f(sllw) f(srlw) f(sraw) f(mulw) f(divw) f(divuw) f(remw) f(remuw) f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu) f(addiw) f(slliw) f(srliw) f(sraiw) f(auipc) f(jalr) f(jal) f(divu) f(div) f(rem) f(remu)

def_all_EXEC_ID();
