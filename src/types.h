#ifndef types_h
#define types_h

#include <stdio.h>
#include <stdbool.h>
#include "../lib/mpc.h"

enum {
  NUMBER_T, ERR_T, SYMBOL_T, S_EXPR_T
};

enum {
  ERR_DIV_ZERO,
  ERR_INVALID_SYMBOL,
  ERR_INVALID_NUMBER,
  ERR_NO_SYMBOL,
  ERR_MUST_BE_NUMBER_SYMBOL,
  ERR_INVALID_ARGS,
  ERR_TOO_MANY_ARGS,
  ERR_INVALID_LIST_ARG,
};

typedef struct types {
  int type;
  long number;
  char *symbol;
  int children_num;
  struct types **children; // pointer to a list of references to "types"
  int err_code;
  char *err;
} types;

types *new_number_t(long number);
types *new_err_t(int err_code, char *err);
types *new_symbol_t(char *symbol);
types *new_s_expr_t();
types *new_quote_t(types *expr);

void s_expr_add_t(types *s_expr, types *t);
void print_t(types *t);
void println_t(types *t);

types *read_number_t(mpc_ast_t *ast);
types *read_t(mpc_ast_t *ast);

void free_t(types *t);
types *take_t(types *t, int i);
types *pop_t(types *t, int i);

#endif //types_h
