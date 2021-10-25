#include "debug.h"
#include <isa.h>

#ifdef CONFIG_ETRACE
static const char *exp_str[] = {"ecall"};
#endif

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   抛出一个号码为NO的异常, 其中epc为触发异常的指令PC, 返回异常处理的出口地址.
   */
  /* 
        将当前PC值保存到mepc寄存器
        在mcause寄存器中设置异常号
        从mtvec寄存器中取出异常入口地址
        跳转到异常入口地址
  */
  #ifdef CONFIG_ETRACE
  printf("\033[31metrace: \033[0mexcep NO: %ld, name: %s\n",NO, exp_str[NO]);
  #endif
  
  cpu.mepc = epc;
  cpu.mcause = 11;
  // switch (NO) {
  //   case -1: //yeild
  //     cpu.mcause = 11;
  //     break;
  //   default:
  //     cpu.mcause = 8;
  // }
  return cpu.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
