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

void parser_parse(env_t *env, char *buffer_out, char *buffer_in) {
  mpc_result_t result;
  if (mpc_parse("<stdin>", buffer_in, Program, &result)) {
    val_t *lisp = eval_t(env, read_t(result.output));
    to_string_t(buffer_out, lisp);
    free_t(lisp);
    mpc_ast_delete(result.output);

  } else {
    buffer_out = mpc_err_string(result.error);
    mpc_err_delete(result.error);
  }
}

void parser_cleanup() {
  const int PARSER_N = 6;
  mpc_cleanup(PARSER_N, Number, Symbol, Expr, Program, S_expr, Quote);
}