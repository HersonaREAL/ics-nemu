#include "common.h"
#include "debug.h"
#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum {
  TK_NOTYPE = 256, TK_EQ,
  TK_DECNUM,
  TK_HEXNUM,
  TK_REG,
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"-", '-'},           // sub
  {"\\*",'*'},          // mul
  {"\\/",'/'},          // chu
  {"\\(", '('},           // left bracket
  {"\\)", ')'},           // right bracket
  {"0x[[:alnum:]]+", TK_HEXNUM},     // hex number
  {"[[:xdigit:]]+", TK_DECNUM},      // dec number
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case '+' : 
          case '-' : 
          case '*' : 
          case '/' : 
          case '(' : 
          case ')' : tokens[nr_token++].type = rules[i].token_type; break;

          case TK_DECNUM : 
          case TK_HEXNUM : {
            if (substr_len > 32) {
              Assert(0,"token length larger than 32!");
            }
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            nr_token++;
          }

          case TK_NOTYPE :  break;
          default: {
            printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
            return false;
          }
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool check_parentheses(int p, int q) {
  printf("p: %d, q: %d\n",p,q);
  Assert(p < q, "check_parentheses error!");
  if (tokens[p].type != '(') {
    return false;
  }

  int lb_num = 0;
  for (int i = p; i < q; ++i) {
    if (tokens[i].type == '(') ++lb_num;
    else if (tokens[i].type == ')') --lb_num;

    if (lb_num <= 0 && i != q-1)
      return false;
  }

  return lb_num == 0;
}

static int getOp(int p, int q) {
  Assert(p < q, "getOp error, p > q!");
  int pos = -1;
  for (int i = p ; i < q && tokens[i].type != '(' ; ++i) {
    if (tokens[i].type == '+' || tokens[i].type == '-') {
      pos = i;
    }

    if (tokens[i].type == '*' || tokens[i].type == '/') {
      if (tokens[pos].type == '+' || tokens[pos].type == '-') 
        continue;
      pos = i;
    }
  }
  Assert(pos != -1, "getOp error!");
  return pos;
}

static word_t eval(int p, int q) {
  if (p > q) {
    Assert(0, "parse error, p > q!");
  }

  if (p == q) {
    switch (tokens[p].type) {
      case TK_DECNUM: 
      case TK_HEXNUM: return strtoull(tokens[p].str,NULL,0);
      case TK_REG: 
      default: Assert(0, "single token error!");
    }
  } else if (check_parentheses(p, q) == true) {
    // surround by (...)
    return eval(p+1, q-1);
  } else {
    int op = getOp(p, q);//the position of 主运算符 in the token expression;
    uint64_t val1 = eval(p, op - 1);
    uint64_t val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: Assert(0, "op error!");
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  
  /* TODO: Insert codes to evaluate the expression. */
  return eval(0, nr_token-1);
}
