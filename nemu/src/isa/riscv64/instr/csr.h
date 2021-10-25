#include "common.h"
#include "debug.h"

#include "cpu/decode.h"
#include "isa.h"
#include "rtl-basic.h"
#include "rtl/rtl.h"
#include <stdint.h>

word_t *zeroNull();

static rtlreg_t *fetchCsr(sword_t csr_idx) {
    switch (csr_idx) {
        case 0x305: // mtvec
            return &cpu.mtvec;
        case 0x341: // mepc
            return &cpu.mepc;
        case 0x300: // mstatus
            return &cpu.mstatus;
        case 0x342: // mcause
            return &cpu.mcause;
        default:
            panic("csr idx error!");
    }
    panic("can not reach here!");
    return NULL; 
}

def_EHelper(csrrw) {
    rtlreg_t *csr = fetchCsr(id_src2->imm);
    uint64_t tmp = *csr;
    *csr = *dsrc1;
    *ddest = tmp;
}

def_EHelper(csrrs) {
    rtlreg_t *csr = fetchCsr(id_src2->imm);
    uint64_t tmp = *csr;
    *csr = *dsrc1 | tmp;
    *ddest = tmp;
}

def_EHelper(ecall) {
    vaddr_t npc = isa_raise_intr(0, cpu.pc);
    rtl_j(s, npc);
}

def_EHelper(mret) {
    /*将 pc 设置为 CSRs[mepc], 将特权级设置成
    CSRs[mstatus].MPP, CSRs[mstatus].MIE 置成 CSRs[mstatus].MPIE, 并且将
    CSRs[mstatus].MPIE 为 1;并且，如果支持用户模式，则将 CSR [mstatus].MPP 设置为 0。
    */
    rtl_jr(s,&cpu.mepc);
    //cpu.mstatus |= (cpu.mstatus >> 4) & 0x8; //CSRs[mstatus].MIE 置成 CSRs[mstatus].MPIE
    //cpu.mstatus |= 0x80;//并且将CSRs[mstatus].MPIE 为 1
}