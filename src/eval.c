#include "eval.h"

types eval_expr(mpc_ast_t *ast);
types bin_op(types x, char *op, types y);

types eval_program(mpc_ast_t *ast) {
  if (ast->children_num == 3) {
    return eval_expr(ast->children[1]);
  } else {
    return eval_expr(ast);
  }
}

types eval_expr(mpc_ast_t *ast) {
  if (strstr(ast->tag, "number")) {
    errno = 0;
    long number = strtol(ast->contents, NULL, 10);
    return errno != ERANGE ? new_tnumber(number) : new_terr(ERR_INVALID_NUMBER);
  }

  char *op = ast->children[1]->contents;
  types x = eval_expr(ast->children[2]);
  if (is_terr(x)) {
    return x;
  }

  for (int i = 3; i < ast->children_num && strstr(ast->children[i]->tag, "expr"); ++i) {
    types y = eval_expr(ast->children[i]);
    if (is_terr(y)) {
      return y;
    }
    x = bin_op(x, op, y);
  }

  return x;
}

types bin_op(types x, char *op, types y) {
  if (strcmp(op, "+") == 0) { return new_tnumber(x.number + y.number); }
  if (strcmp(op, "-") == 0) { return new_tnumber(x.number - y.number); }
  if (strcmp(op, "*") == 0) { return new_tnumber(x.number * y.number); }
  if (strcmp(op, "/") == 0) {
    return (y.number == 0) ? new_terr(ERR_DIV_ZERO) : new_tnumber(x.number / y.number);
  }
  return new_terr(ERR_INVALID_OP);
}