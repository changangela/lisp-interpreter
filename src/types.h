#ifndef types_h
#define types_h

#include <stdio.h>
#include <stdbool.h>
#include "../lib/mpc.h"

enum { NUMBER_T, ERR_T, SYMBOL_T, S_EXPR_T, QUOTE_T };
enum {
  ERR_DIV_ZERO,
  ERR_INVALID_SYMBOL,
  ERR_INVALID_NUMBER,
  ERR_NO_SYMBOL,
  ERR_MUST_BE_NUMBER_SYMBOL,
  ERR_INVALID_ARGS_NUMBER,
  ERR_INVALID_ARGS_TYPE,
  ERR_INVALID_ARGS,
};

typedef struct types {
  int type;
  long number;
  char *symbol;
  // pointer to a list of references to "types"
  int children_num;
  struct types** children;
  struct types *quote;
  int err;

  char *func_name;
} types;

types *new_number_t(long number);
types *new_err_t(int err_code);
types *new_func_err_t(int err_code, char *func_name);
types *new_symbol_t(char *symbol);
types *new_s_expr_t();
types *new_quote_t(types *expr);

void s_expr_add_t(types *s_expr, types *t);

bool is_err_t(types *t);
void print_t(types *t);
void println_t(types *t);

types *read_number_t(mpc_ast_t *ast);
types *read_t(mpc_ast_t *ast);

void free_t(types *t);
types *take_t(types *t, int i);
types *pop_t(types *t, int i);

#endif //types_h
