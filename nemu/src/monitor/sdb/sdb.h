#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>
#include <stdint.h>
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  uint64_t initRes;
  char *expr_str;
} WP;

word_t expr(char *e, bool *success);

#endif
