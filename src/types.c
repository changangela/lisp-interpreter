#include <stdlib.h>
#include "types.h"

const char *err_fmt_t(int err_code);

val_t *new_number_t(long number) {
  val_t *ret = malloc(sizeof(val_t));
  ret->type = NUMBER_T;
  ret->number = number;
  return ret;
}

val_t *new_err_t(int err_code, ...) {
  val_t *ret = malloc(sizeof(val_t));
  ret->type = ERR_T;

  va_list va;
  va_start(va, err_code);

  int ERR_SIZE = 1 << 10;
  ret->err = malloc(ERR_SIZE);

  vsnprintf(ret->err, ERR_SIZE - 1, err_fmt_t(err_code), va);

  ret->err = realloc(ret->err, strlen(ret->err) + 1);

  va_end(va);

  return ret;
}

val_t *new_symbol_t(char *symbol) {
  val_t *ret = malloc(sizeof(val_t));
  ret->type = SYMBOL_T;
  ret->symbol = malloc(strlen(symbol) + 1);
  strcpy(ret->symbol, symbol);
  return ret;
}

val_t *new_s_expr_t() {
  val_t *ret = malloc(sizeof(val_t));
  ret->type = S_EXPR_T;
  ret->children_num = 0;
  ret->children = NULL;
  return ret;
}

val_t *new_quote_t(val_t *expr) {
  val_t *ret = malloc(sizeof(val_t));
  ret->type = S_EXPR_T;
  ret->children_num = 0;
  ret->children = NULL;

  val_t *quote = new_symbol_t("quote");
  s_expr_add_t(ret, quote);
  s_expr_add_t(ret, expr);

  return ret;
}

val_t *new_func_t(builtin_t *func) {
  val_t *ret = malloc(sizeof(val_t));
  ret->type = FUNC_T;
  ret->func = func;
  return ret;
}

void s_expr_add_t(val_t *s_expr, val_t *t) {
  s_expr->children_num++;
  s_expr->children = realloc(s_expr->children,
    sizeof(val_t *) * s_expr->children_num);
  s_expr->children[s_expr->children_num - 1] = t;
}

const char *err_fmt_t(int err_code) {
  switch (err_code) {
    case ERR_UNBOUND_SYMBOL: return "error: unbounded symbol '%s'";
    case ERR_DIV_ZERO: return "error: division by zero";
    case ERR_INVALID_NUMBER: return "error: '%s' is not a valid number";
    case ERR_TOO_MANY_ARGS: return "error: too many arguments passed to function '%s'";
    case ERR_NOT_ENOUGH_ARGS: return "error: too few arguments passed to function '%s'";
    case ERR_INVALID_ARG_TYPE: return "error: function '%s' passed invalid type for argument %d: found %s, expected %s";
    case ERR_INVALID_ARG_VALUE: return "error: function '%s' passed invalid value for argument %d: found %s, expected %s";
  }
}

const char *type_name(int type) {
  switch (type) {
    case NUMBER_T: return "number";
    case ERR_T: return "error";
    case SYMBOL_T: return "symbol";
    case S_EXPR_T: return "list";
    case FUNC_T: return "function";
    default: return "unknown";
  }
}

void print_err_t(val_t *t) {
  printf(t->err);
}

void print_s_expr_t(val_t *t) {
  putchar('(');
  for (int i = 0; i < t->children_num; ++i) {
    print_t(t->children[i]);

    if (i < t->children_num - 1) {
      putchar(' ');
    }
  }
  putchar(')');
}

void print_expr_t(val_t *t) {
  switch (t->type) {
    case NUMBER_T: printf("%li", t->number);
      return;
    case SYMBOL_T: printf("%s", t->symbol);
      return;
    case S_EXPR_T: print_s_expr_t(t);
      return;
    case ERR_T: print_err_t(t);
      return;
    case FUNC_T: printf("<function>");
  }
}

void print_t(val_t *t) {
  switch (t->type) {
    default: print_expr_t(t);
  }
}

void println_t(val_t *t) {
  print_t(t);
  printf("\n");
}

val_t *read_number_t(mpc_ast_t *ast) {
  errno = 0;
  long number = strtol(ast->contents, NULL, 10);
  return errno != ERANGE ? new_number_t(number) : new_err_t(ERR_INVALID_NUMBER,
    ast->contents);
}

val_t *read_t(mpc_ast_t *ast) {
  if (strstr(ast->tag, "number")) {
    return read_number_t(ast);
  }

  if (strstr(ast->tag, "symbol")) {
    return new_symbol_t(ast->contents);
  }

  val_t *ret = NULL;

  if (strcmp(ast->tag, ">") == 0 || strstr(ast->tag, "s_expr")) {
    ret = new_s_expr_t();
    for (int i = 0; i < ast->children_num; ++i) {
      if (strcmp(ast->children[i]->contents, "(") == 0 ||
        strcmp(ast->children[i]->contents, ")") == 0 ||
        strcmp(ast->children[i]->tag, "regex") == 0) {
        continue;
      }
      s_expr_add_t(ret, read_t(ast->children[i]));
    }
  }

  if (strstr(ast->tag, "quote")) {
    val_t *quote = read_t(ast->children[1]);
    if (quote->type == ERR_T) {
      free(ret);
      return quote;
    }
    ret = new_quote_t(quote);
  }

  return ret;
}

void free_t(val_t *t) {
  switch (t->type) {
    case SYMBOL_T: free(t->symbol);
      break;
    case ERR_T: free(t->err);
      break;
    case S_EXPR_T:
      for (int i = 0; i < t->children_num; ++i) {
        free_t(t->children[i]);
      }
      free(t->children);
  }
  free(t);
}

val_t *take_t(val_t *t, int i) {
  val_t *target = pop_t(t, i);
  free_t(t);
  return target;
}

val_t *pop_t(val_t *t, int i) {
  val_t *target = t->children[i];
  memmove(&t->children[i], &t->children[i + 1],
    sizeof(val_t *) * (t->children_num - i - 1));
  t->children_num--;
  t->children = realloc(t->children, sizeof(val_t *) * t->children_num);
  return target;
}

void *join_t(val_t *head, val_t *tail) {
  while (tail->children_num > 0) {
    s_expr_add_t(head, pop_t(tail, 0));
  }
  free_t(tail);
}

val_t *copy_t(val_t *t) {
  val_t *ret = malloc(sizeof(val_t));
  ret->type = t->type;

  switch (t->type) {
    case FUNC_T: ret->func = t->func;
      break;
    case NUMBER_T: ret->number = t->number;
      break;
    case SYMBOL_T:ret->symbol = malloc(strlen(t->symbol) + 1);
      strcpy(ret->symbol, t->symbol);
      break;
    case ERR_T:ret->err = malloc(strlen(t->err));
      strcpy(ret->err, t->err);
      break;
    case S_EXPR_T:ret->children_num = t->children_num;
      ret->children = malloc(sizeof(val_t *) * ret->children_num);
      for (int i = 0; i < t->children_num; ++i) {
        ret->children[i] = copy_t(t->children[i]);
      }
      break;
  }

  return ret;
}
