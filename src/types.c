#include <stdlib.h>
#include "types.h"
#include "env.h"

void to_string_val_t(char *buffer, val_t *t);
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

val_t *new_func_t(builtin_t func) {
  val_t *ret = malloc(sizeof(val_t));
  ret->type = FUNC_T;
  ret->func = func;
  return ret;
}

val_t *new_named_lambda_t(val_t *formals, val_t *body, char *name) {
  val_t *ret = new_lambda_t(formals, body);

  ret->name = malloc(strlen(name) + 1);
  strcpy(ret->name, name);

  return ret;
}

val_t *new_lambda_t(val_t *formals, val_t *body) {
  val_t *ret = malloc(sizeof(val_t));
  ret->type = LAMBDA_T;

  ret->env = new_env_t();
  ret->formals = formals;
  ret->body = body;
  ret->name = NULL;

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
    case ERR_TYPE_MISMATCH: return "error: expecting a %s, found %s";
    case ERR_INVALID_LAMBDA_FORMALS: return "error: invalid lambda list element %d: found %s, expected %s";
  }
  return NULL;
}

const char *type_name_t(int type) {
  switch (type) {
    case NUMBER_T: return "number";
    case ERR_T: return "error";
    case SYMBOL_T: return "symbol";
    case S_EXPR_T: return "list";
    case FUNC_T: return "function";
    default: return "unknown";
  }
}

void to_string_err_t(char *ret, val_t *t) {
  sprintf(ret + strlen(ret), "%s", t->err);
}

void to_string_s_expr_t(char *ret, val_t *t) {
  sprintf(ret + strlen(ret), "(");
  for (int i = 0; i < t->children_num; ++i) {
    to_string_val_t(ret, t->children[i]);

    if (i < t->children_num - 1) {
      sprintf(ret + strlen(ret), " ");
    }
  }
  sprintf(ret + strlen(ret), ")");
}

void to_string_val_t(char *buffer, val_t *t) {
  switch (t->type) {
    case NUMBER_T: sprintf(buffer + strlen(buffer), "%li", t->number);
      return;
    case SYMBOL_T: sprintf(buffer + strlen(buffer), "%s", t->symbol);
      return;
    case S_EXPR_T: to_string_s_expr_t(buffer, t);
      return;
    case ERR_T: to_string_err_t(buffer, t);
      return;
    case FUNC_T: sprintf(buffer + strlen(buffer), "<function>");
      return;
    case LAMBDA_T: {
      sprintf(buffer + strlen(buffer), "(%s ",
        t->name == NULL ? "lambda" : t->name);
      to_string_val_t(buffer, t->formals);
      sprintf(buffer + strlen(buffer), " ");
      to_string_val_t(buffer, t->body);
      sprintf(buffer + strlen(buffer), ")");
      return;
    }
  }
}

void to_string_t(char *buffer, val_t *t) {
  to_string_val_t(buffer, t);
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
      free_t(ret);
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
      break;
    case LAMBDA_T: {
      free_env_t(t->env);
      free_t(t->formals);
      free_t(t->body);
      if (t->name != NULL) {
        free(t->name);
      }
      break;
    }
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

void join_t(val_t *head, val_t *tail) {
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
    case SYMBOL_T: ret->symbol = malloc(strlen(t->symbol) + 1);
      strcpy(ret->symbol, t->symbol);
      break;
    case ERR_T: ret->err = malloc(strlen(t->err));
      strcpy(ret->err, t->err);
      break;
    case S_EXPR_T: ret->children_num = t->children_num;
      ret->children = malloc(sizeof(val_t *) * ret->children_num);
      for (int i = 0; i < t->children_num; ++i) {
        ret->children[i] = copy_t(t->children[i]);
      }
      break;
    case LAMBDA_T: {
      ret->env = copy_env_t(t->env);
      ret->formals = copy_t(t->formals);
      ret->body = copy_t(t->body);
      if (t->name == NULL) {
        ret->name = NULL;
      } else {
        ret->name = malloc(strlen(t->name) + 1);
        strcpy(ret->name, t->name);
      }
      break;
    }
  }

  return ret;
}
