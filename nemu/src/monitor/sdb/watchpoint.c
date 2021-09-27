#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sdb.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].expr_str = NULL;
    wp_pool[i].initRes = 0;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
bool debug_wp() {
  WP *cur = head;
  while(cur) {
    bool success = false;
    uint64_t res = expr(cur->expr_str, &success);
    assert(success);
    if (res != cur->initRes) {
      printf("\033[32mhit watch point!\033[33m\nNO.%d\033[0m: %s \033[31m-->\033[0m %ld, 0x%lx\n", cur->NO, cur->expr_str, res, res);
      return true;
    }
    cur = cur->next;
  }
  return false;
}

void list_wp() {
  printf("\033[32mwatch point list:\033[0m \n");
  WP *cur = head;
  while(cur) {
    printf("\033[31mNO.%d\033[0m: %s\n",cur->NO, cur->expr_str);
    cur = cur->next;
  }
}

void free_wp(int NO) {
  WP *help = (WP *)malloc(sizeof(WP));
  WP *prev = help;
  prev->next = head;
  while (prev->next) {
    if (prev->next->NO == NO) {
      WP *wp = prev->next;
      if (wp->expr_str) {
        free(wp->expr_str);
        wp->expr_str = NULL;
        wp->initRes = 0;
      }

      // remove node from head
      prev->next = wp->next;
      head = help->next;
      free(help);

      // add node to free_
      wp->next = free_;
      free_ = wp;

      return;
    }

    prev = prev->next;
  }

  assert(0);
}

WP* new_wp(char *args) {
  if (free_ == NULL) {
    return NULL;
  }

  //remove node from free_
  WP* ret = free_;
  free_ = free_->next;

  //add node to head
  ret->next = head;
  head = ret;

  //alloc str
  int n = strlen(args);
  ret->expr_str = (char *)malloc(n+1);
  strcpy(ret->expr_str, args);
  ret->expr_str[n] = '\0';

  bool success = false;
  ret->initRes = expr(ret->expr_str, &success);

  if (!success) {
    free_wp(ret->NO);
    return NULL;
  }
  return ret;
}



