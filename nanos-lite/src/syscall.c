#include <common.h>
#include "debug.h"
#include "syscall.h"

#if config_strace
char *sys_str[] = {"sys_exit", "sys_yield","sys_open","sys_read","sys_write"};
#endif

uintptr_t sys_write(int fd, void *buf, size_t count) {
  int i;
  if (fd == 1 || fd == 2) {
    //just std or err
    char *p = (char *)buf;
    for (i = 0; i < count; ++i) {
      putch(p[i]);
    }
    return i + 1;

  } else {
    panic("write not imp!");
  }
}

uintptr_t sys_brk(void *pb) {
  return 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

#if config_strace
  printf("\033[33mstrace:\033[0m %s\n",sys_str[a[0]]);
#endif

  switch (a[0]) {
    case SYS_yield :
              yield();
              c->GPRx = 0;
              break;
    case SYS_exit :
              halt(c->GPR2);
              break;
    case SYS_write:
              c->GPRx = sys_write(c->GPR2, (void *)c->GPR3, c->GPR4);
              break;
    case SYS_brk:
              c->GPRx = sys_brk((void *)c->GPR2);
              break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
