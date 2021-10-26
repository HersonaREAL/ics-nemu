#include <common.h>
#include <fs.h>
#include <stdint.h>
#include "debug.h"
#include "syscall.h"

#if config_strace
char *sys_str[] = {  "SYS_exit",
  "SYS_yield",
  "SYS_open",
  "SYS_read",
  "SYS_write",
  "SYS_kill",
  "SYS_getpid",
  "SYS_close",
  "SYS_lseek",
  "SYS_brk",
  "SYS_fstat",
  "SYS_time",
  "SYS_signal",
  "SYS_execve",
  "SYS_fork",
  "SYS_link",
  "SYS_unlink",
  "SYS_wait",
  "SYS_times",
  "SYS_gettimeofday"};
#endif

struct timeval {
  long tv_sec;     /* seconds */
  long tv_usec;    /* microseconds */
};

struct timezone {
  int tz_minuteswest;     /* minutes west of Greenwich */
  int tz_dsttime;         /* type of DST correction */
};


uintptr_t sys_write(int fd, void *buf, size_t count) {
  #if config_strace
  printf("\033[33mstrace:\033[0m sys_write, file name: %s\n",fs_filename(fd));
  #endif
  return fs_write(fd, buf, count);
}

uintptr_t sys_read(int fd, void *buf, size_t len) {
  #if config_strace
  printf("\033[33mstrace:\033[0m sys_read, file name: %s\n",fs_filename(fd));
  #endif
  return fs_read(fd, buf, len);
}

uintptr_t sys_open(const char *pathname, int flags, int mode) {
  #if config_strace
  printf("\033[33mstrace:\033[0m sys_open, file name: %s\n",fs_filename(fd));
  #endif
  return fs_open(pathname, flags, mode);
}

uintptr_t sys_lseek(int fd, size_t offset, int whence) {
  #if config_strace
  printf("\033[33mstrace:\033[0m sys_lseek, file name: %s\n",fs_filename(fd));
  #endif
  return fs_lseek(fd, offset, whence);
}

uintptr_t sys_close(int fd) {
  #if config_strace
  printf("\033[33mstrace:\033[0m sys_close, file name: %s\n",fs_filename(fd));
  #endif
  return fs_close(fd);
}

uintptr_t sys_brk(void *pb) {
  return 0;
}

uintptr_t sys_gettimeofday(struct timeval *tv, struct timezone *tz) {
  long us = io_read(AM_TIMER_UPTIME).us;
  if (us >= 1000000) {
    tv->tv_sec = us / 1000000;
    tv->tv_usec = us % 1000000;
  } else {
    tv->tv_sec = 0;
    tv->tv_usec = us;
  }
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
    case SYS_read:
              c->GPRx = sys_read(c->GPR2, (void *)c->GPR3, c->GPR4);
              break;
    case SYS_lseek:
              c->GPRx = sys_lseek(c->GPR2, c->GPR3, c->GPR4);
              break;
    case SYS_open :
              c->GPRx = sys_open((const char *)c->GPR2, c->GPR3, c->GPR4);
              break;
    case SYS_close:
              c->GPRx = sys_close(c->GPR2);
              break;
    case SYS_gettimeofday:
              c->GPRx = sys_gettimeofday((struct timeval*)c->GPR2, (struct timezone *)c->GPR3);
              break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
