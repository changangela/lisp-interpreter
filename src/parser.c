#include "parser.h"
#include "eval.h"

void parser_init() {
  Number = mpc_new("number");
  Op = mpc_new("op");
  Expr = mpc_new("expr");
  Program = mpc_new("program");

  mpca_lang(
    MPCA_LANG_DEFAULT,
    "number: /-?[0-9]+/;"
    "op: '+' | '-' | '*' | '/' ;"
    "expr: <number> | '(' <op> <expr>+ ')' ;"
    "program: /^/ <op> <expr>+ /$/ | /^/ <expr> /$/;",
    Number, Op, Expr, Program
  );
}

void parser_parse(char *buffer) {
  mpc_result_t result;

  if (mpc_parse("<stdin>", buffer, Program, &result)) {
    tprintln(eval_program(result.output));
    mpc_ast_delete(result.output);
  } else {
    mpc_err_print(result.error);
    mpc_err_delete(result.error);
  }
}

void parser_cleanup() {
  const int PARSER_N = 4;
  mpc_cleanup(PARSER_N, Number, Op, Expr, Program);
}