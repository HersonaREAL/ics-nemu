#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include "common.h"
#include "memory/paddr.h"
#include "sdb.h"
#include "utils.h"

static int is_batch_mode = false;
void init_regex();
void init_wp_pool();
WP* new_wp(char *args);
void free_wp(int NO);
void list_wp();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_END;
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  /* TODO: Add more commands */
  { "si", "pc run n steps", cmd_si },
  { "info", "r print info of reg, w print info of watch point", cmd_info},
  { "x", "scan mem, x N expr", cmd_x },
  { "p", "print value of expr", cmd_p },
  { "w", "add watch point", cmd_w },
  { "d", "delete watch point", cmd_d },
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
  char *n_str = strtok(NULL, " ");
  int n = n_str == NULL ? 1 :  atoi(n_str);
  if (n <= 0) {
    printf("n must be larger than 0!\n");
    return 0;
  }

  cpu_exec(n);
  
  return 0;
}

static int cmd_info(char *args) {
  char *str = strtok(NULL, " ");
  if (str == NULL || strcmp(str,"r") == 0) {
    isa_reg_display();
  } else if (strcmp(str,"w") == 0) {
    // TODO print watch point
    list_wp();
  } else {
    printf("\033[31merror args for info\033[0m\n");
  }
  return 0;
}

static int cmd_x(char *args) {
  char *n_str = strtok(NULL," ");
  char *expr_str = strtok(NULL," ");
  if (!n_str || !expr_str) {
    printf("\033[31mplease input x n expr!\033[0m\n");
    return 0;
  }

  int n = atoi(n_str);
  if (n <= 0) {
    printf("\033[31mn must be larger than 0!\033[0m\n");
  }
  
  bool success = false;
  paddr_t addr =  expr(expr_str,&success);

  if (!success) {
    printf("\033[31merror addr!\033[0m\n");
    return 0;
  }
  printf("%s: ", expr_str);
  for (int i = 0; i < n; ++i) {
    word_t val =  paddr_read(addr + (i<<2), 4);
    printf("0x%08lx\t",val);
  }
  printf("\n");
  return 0;
}

static int cmd_p(char *args) {
  bool res = false;
  uint64_t val = expr(args,&res);
  if (!res) {
    printf("expr invaild!\n");
    return 0;
  }

  printf("%s: \033[32m%ld\033[0m, \033[33m0x%08lx\033[0m\n",args,val,val);
  return 0;
}

static int cmd_w(char *args) {
  WP *wp = new_wp(args);
  if (!wp) {
    printf("alloc watch point fail!\n");
  }
  printf("\033[32malloc watch point success!\n\033[33mNo: %d\033[0m, expr: %s\n",wp->NO, wp->expr_str);
  return 0;
}

static int cmd_d(char *args) {
  char *n_str = strtok(NULL, " ");
  if (n_str == NULL) {
    return cmd_help(NULL);
  }
  int n = atoi(n_str);
  if (n < 0 || n >= 32) {
    printf("n error\n");
    return 0;
  }

  free_wp(n);
  
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
