#ifndef types_h
#define types_h

#include <stdio.h>
#include <stdbool.h>
#include "../lib/mpc.h"
#include "builtins.h"

enum {
  NUMBER_T, ERR_T, SYMBOL_T, S_EXPR_T, FUNC_T
};

enum {
  ERR_DIV_ZERO,
  ERR_INVALID_NUMBER,
  ERR_INVALID_ARG_TYPE,
  ERR_INVALID_ARG_VALUE,
  ERR_TOO_MANY_ARGS,
  ERR_UNBOUND_SYMBOL,
  ERR_NOT_ENOUGH_ARGS,
  ERR_TYPE_MISMATCH,
};

struct val_t {
  int type;
  long number;
  char *symbol;
  int children_num;
  struct val_t **children; // pointer to a list of references to "val_t"
  builtin_t func;
  char *err;
};

val_t *new_number_t(long number);
val_t *new_err_t(int err_code, ...);
val_t *new_symbol_t(char *symbol);
val_t *new_s_expr_t();
val_t *new_quote_t(val_t *expr);
val_t *new_func_t(builtin_t *func);

void s_expr_add_t(val_t *s_expr, val_t *t);
const char *type_name(int type);

void print_t(val_t *t);
void println_t(val_t *t);

val_t *read_number_t(mpc_ast_t *ast);
val_t *read_t(mpc_ast_t *ast);

void free_t(val_t *t);
val_t *take_t(val_t *t, int i);
val_t *pop_t(val_t *t, int i);
void *join_t(val_t *head, val_t *tail);
val_t *copy_t(val_t *t);

#endif //types_h
