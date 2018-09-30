#include <stdlib.h>
#include "types.h"

types *new_number_t(long number) {
  types *ret = malloc(sizeof(types));
  ret->type = NUMBER_T;
  ret->number = number;
  return ret;
}

types *new_err_t(int err_code) {
  types *ret = malloc(sizeof(types));
  ret->type = ERR_T;
  ret->err = err_code;
  return ret;
}

types *new_symbol_t(char *symbol) {
  types *ret = malloc(sizeof(types));
  ret->type = SYMBOL_T;
  ret->symbol = malloc(strlen(symbol) + 1);
  strcpy(ret->symbol, symbol);
  return ret;
}

types *new_s_expr_t() {
  types *ret = malloc(sizeof(types));
  ret->type = S_EXPR_T;
  ret->children_num = 0;
  ret->children = NULL;
  return ret;
}

types *new_quote_t(types *expr) {
  types *ret = malloc(sizeof(types));
  ret->type = QUOTE_T;
  ret->quote = expr;
  return ret;
}

void s_expr_add_t(types *s_expr, types *t) {
  s_expr->children_num++;
  s_expr->children = realloc(s_expr->children,
                             sizeof(types *) * s_expr->children_num);
  s_expr->children[s_expr->children_num - 1] = t;


}

bool is_err_t(types *t) {
  return t->type == ERR_T;
}

void print_err_t(types *t) {
  switch (t->err) {
    case ERR_DIV_ZERO:
      printf("error: divide by zero");
      return;
    case ERR_INVALID_SYMBOL:
      printf("error: invalid operation");
      return;
    case ERR_INVALID_NUMBER:
      printf("error: invalid number");
      return;
    case ERR_NO_SYMBOL:
      printf("error: symbol not found at start of s-expression");
      return;
    case ERR_MUST_BE_NUMBER_SYMBOL:
      printf("error: cannot perform operation on non-number");
      return;
  }
}

void print_s_expr_t(types *t) {
  putchar('(');
  for (int i = 0; i < t->children_num; ++i) {
    print_t(t->children[i]);

    if (i < t->children_num - 1) {
      putchar(' ');
    }
  }
  putchar(')');
}

void print_expr_t(types *t) {
  switch (t->type) {
    case NUMBER_T:
      printf("%li", t->number);
      return;

    case SYMBOL_T:
      printf("%s", t->symbol);
      return;

    case QUOTE_T:
      printf("'");
      print_t(t->quote);
      return;

    case S_EXPR_T:
      print_s_expr_t(t);
      return;

    case ERR_T:
      print_err_t(t);
      return;
  }
}

void print_t(types *t) {
  switch(t->type) {
    case QUOTE_T:
      print_expr_t(t->quote);
      return;
    default:
      print_expr_t(t);
  }
}

void println_t(types *t) {
  print_t(t);
  printf("\n");
}

types *read_number_t(mpc_ast_t *ast) {
  errno = 0;
  long number = strtol(ast->contents, NULL, 10);
  return errno != ERANGE ? new_number_t(number) : new_err_t(ERR_INVALID_NUMBER);
}

types *read_t(mpc_ast_t *ast) {
  if (strstr(ast->tag, "number")) { return read_number_t(ast); }
  if (strstr(ast->tag, "symbol")) { return new_symbol_t(ast->contents); }

  types *ret = NULL;

  if (strcmp(ast->tag, ">") == 0 || strstr(ast->tag, "s_expr")) {
    ret = new_s_expr_t();
    for (int i = 0; i < ast->children_num; ++i) {
      if (strcmp(ast->children[i]->contents, "(") == 0 ||
          strcmp(ast->children[i]->contents, ")") == 0 ||
          strcmp(ast->children[i]->tag, "regex") == 0) { continue; }
      s_expr_add_t(ret, read_t(ast->children[i]));
    }
  }

  if (strstr(ast->tag, "quote")) {
    types *quote = NULL;
    if (ast->children_num == 4) {
      // ( quote <expr> )
      quote = read_t(ast->children[2]);
    } else if (ast->children_num == 2) {
      // ' <expr>
      quote = read_t(ast->children[1]);
    }
    if (quote->type == ERR_T) { free(ret); return quote; }
    ret = new_quote_t(quote);
  }


  return ret;
}

void free_t(types *t) {
  switch (t->type) {
    case SYMBOL_T:
      free(t->symbol);
      break;

    case QUOTE_T: free(t->quote); break;
    case S_EXPR_T:
      for (int i = 0; i < t->children_num; ++i) {
        free_t(t->children[i]);
      }
      free(t->children);
  }
  free(t);
}

types *take_t(types *t, int i) {
  types *target = pop_t(t, i);
  free_t(t);
  return target;
}

types *pop_t(types *t, int i) {
  types *target = t->children[i];
  memmove(&t->children[i], &t->children[i + 1],
          sizeof(types *) * (t->children_num - i - 1));
  t->children_num--;
  t->children = realloc(t->children, sizeof(types *) * t->children_num);
  return target;
}