#include "parser.h"

void parser_init() {
  Number = mpc_new("number");
  Symbol = mpc_new("symbol");
  Expr = mpc_new("expr");
  Program = mpc_new("program");
  S_expr = mpc_new("s_expr");
  Quote = mpc_new("quote");

  mpca_lang(
    MPCA_LANG_DEFAULT,
    "number: /-?[0-9]+/;"
    "symbol: /[a-zA-Z0-9_+\\-*\\/]+/ ;"
    "quote: '\'' <expr> ;"
    "s_expr: '(' <expr>* ')' ;"
    "expr: <number> | <symbol> | <s_expr> | <quote> ;"
    "program: /^/ <expr>* /$/;",
    Number, Symbol, Expr, S_expr, Program, Quote
  );
}

void parser_parse(env_t *env, char *buffer) {
  mpc_result_t result;

  if (mpc_parse("<stdin>", buffer, Program, &result)) {
    val_t *lisp = eval_t(env, read_t(result.output));
    println_t(lisp);
    free_t(lisp);
    mpc_ast_delete(result.output);

  } else {
    mpc_err_print(result.error);
    mpc_err_delete(result.error);
  }
}

void parser_cleanup() {
  const int PARSER_N = 6;
  mpc_cleanup(PARSER_N, Number, Symbol, Expr, Program, S_expr, Quote);
}