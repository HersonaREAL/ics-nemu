#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%d\", result); "
"  return 0; "
"}";

static unsigned int bp = 0;

static int choose(int k) {
  return rand() % k;
}

static void genRandSpace() {
  int space = rand() % 2;
  for (int i = 0; i < space; ++i)
    buf[bp++] = ' ';
}

static void gen_num(bool canBeZero) {
  genRandSpace();
  uint32_t num = rand()%1000;
  while(!canBeZero && num == 0) {
    num = rand();
  }
  int n = 0;
  switch(choose(2)) {
    case 0:
      n = sprintf(buf+bp, "%u ", num);
      break;
    case 1:
      n = sprintf(buf+bp, "0x%x ", num);
      break;
    default: assert(0);
  }
  bp += n;
  genRandSpace();
}

static void gen(char c) {
  genRandSpace();
  buf[bp++] = c;
  genRandSpace();
}

static void gen_rand_op(bool canbeZero) {
  genRandSpace();
  switch(choose(3)) {
    case 0: buf[bp++] = '+'; break;
    case 1: buf[bp++] = '-';
      break;
    case 2: buf[bp++] = '*'; break;
    case 3: 
      if (canbeZero)
        buf[bp++] = '/';
      else
        buf[bp++] = '+';
      break;
    default:assert(0);
  }
  genRandSpace();
}

static void gen_rand_expr(bool canBeZero) {
  if (bp + 5000 > sizeof(buf)) {
    return gen_num(canBeZero);
  }

  switch (choose(3)) {
    case 0: gen_num(canBeZero); break;
    case 1: gen('('); gen_rand_expr(canBeZero); gen(')'); break;
    default: gen_rand_expr(canBeZero); gen_rand_op(canBeZero);gen_rand_expr(buf[bp-1] != '/'); break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    memset(buf,0,sizeof(buf));
    bp = 0;
    gen_rand_expr(true);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
