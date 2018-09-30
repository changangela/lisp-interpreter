#include "eval.h"

types *eval_s_expr_t(types *t);

types *builtin_t(types *symbol, types *args);

types *builtin_binop_t(char *op, types *args);

types *builtin_list_t(types *args);

types *eval_t(types *t) {
  switch (t->type) {
    case S_EXPR_T:
      return eval_s_expr_t(t);
    default:
      return t;
  }
}

types *eval_s_expr_t(types *t) {
  for (int i = 0; i < t->children_num; ++i) {
    t->children[i] = eval_t(t->children[i]);
    if (t->children[i]->type == ERR_T) {
      return take_t(t, i);
    }
  }

  if (t->children_num == 0) return t;
  if (t->children_num == 1) return t->children[0];

  types *symbol = pop_t(t, 0);
  if (symbol->type != SYMBOL_T) {
    free_t(t);
    free_t(symbol);
    return new_err_t(ERR_NO_SYMBOL);
  }

  types *result = builtin_t(symbol, t);
  free_t(symbol);

  return result;
}

types *builtin_t(types *func, types *args) {
  if (strcmp("list", func->symbol) == 0) { return builtin_list_t(args); }
  if (strstr("+-*/", func->symbol)) {
    return builtin_binop_t(func->symbol, args);
  }

  free_t(args);
  return new_err_t(ERR_INVALID_SYMBOL);
}

types *builtin_list_t(types *args) {
  return new_quote_t(args);
}

types *builtin_binop_t(char *op, types *args) {
  for (int i = 0; i < args->children_num; ++i) {
    if (args->children[i]->type != NUMBER_T) {
      free_t(args);
      return new_err_t(ERR_MUST_BE_NUMBER_SYMBOL);
    }
  }

  types *head = pop_t(args, 0);

  if (strcmp(op, "-") == 0 && args->children_num == 0) {
    head->number = -head->number;
  }

  while (args->children_num > 0) {
    types *current = pop_t(args, 0);
    if (strcmp(op, "+") == 0) { head->number += current->number; }
    if (strcmp(op, "-") == 0) { head->number -= current->number; }
    if (strcmp(op, "*") == 0) { head->number *= current->number; }
    if (strcmp(op, "/") == 0) {
      if (current->number == 0) {
        free_t(head);
        free_t(current);
        return new_err_t(ERR_DIV_ZERO);
      }
      head->number /= current->number;
    }
    free_t(current);
  }

  free_t(args);

  return head;
}