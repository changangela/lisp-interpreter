#include "eval.h"

types *eval_s_expr_t(types *t);
types *builtin_t(types *symbol, types *args);
types *builtin_binop_t(char *op, types *args);
types *builtin_list_t(types *args);
types *builtin_car_t(types *args);
types *builtin_cdr_t(types *args);
types *builtin_quote_t(types *args);

types *eval_t(types *t) {
  switch (t->type) {
    case S_EXPR_T: return eval_s_expr_t(t);
    default: return t;
  }
}

types *eval_s_expr_t(types *t) {
  if (t->children_num == 0) return t;
  if (t->children_num == 1 && t->children[0]->type != SYMBOL_T)
    return eval_t(t->children[0]);

  types *symbol = pop_t(t, 0);
  if (symbol->type != SYMBOL_T) {
    free_t(t);
    free_t(symbol);
    return new_err_t(ERR_NO_SYMBOL, "");
  }

  types *result = builtin_t(symbol, t);
  free_t(symbol);

  return result;
}

types *builtin_t(types *func, types *args) {
  if (strcmp("quote", func->symbol) == 0) { return builtin_quote_t(args); }

  for (int i = 0; i < args->children_num; ++i) {
    args->children[i] = eval_t(args->children[i]);
    if (args->children[i]->type == ERR_T) {
      return take_t(args, i);
    }
  }

  if (strcmp("list", func->symbol) == 0) { return builtin_list_t(args); }
  if (strcmp("car", func->symbol) == 0) { return builtin_car_t(args); }
  if (strcmp("cdr", func->symbol) == 0) { return builtin_cdr_t(args); }
  if (strstr("+-*/", func->symbol)) {
    return builtin_binop_t(func->symbol, args);
  }

  free_t(args);
  return new_err_t(ERR_INVALID_SYMBOL, func->symbol);
}

types *builtin_list_t(types *args) {
  return args;
}

types *builtin_binop_t(char *op, types *args) {
  for (int i = 0; i < args->children_num; ++i) {
    assert_t(args->children[i]->type == NUMBER_T, args,
             ERR_MUST_BE_NUMBER_SYMBOL, "");
  }

  types *head = args->children_num > 0 ? pop_t(args, 0) : new_number_t(0);

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
        return new_err_t(ERR_DIV_ZERO, "");
      }
      head->number /= current->number;
    }
    free_t(current);
  }

  free_t(args);
  return head;
}

types *builtin_car_t(types *args) {
  assert_t(args->children_num == 1, args, ERR_TOO_MANY_ARGS, "car");
  assert_t(args->children[0]->type == S_EXPR_T,
           args,
           ERR_INVALID_LIST_ARG,
           "car");
  assert_t(args->children[0]->children_num >= 1, args, ERR_INVALID_ARGS,
           "empty list passed to function 'car'");

  types *head = pop_t(args->children[0], 0);
  free_t(args);
  return head;
}

types *builtin_cdr_t(types *args) {
  assert_t(args->children_num == 1, args, ERR_TOO_MANY_ARGS, "cdr");
  assert_t(args->children[0]->type == S_EXPR_T,
           args,
           ERR_INVALID_LIST_ARG,
           "cdr");

  types *list = take_t(args, 0);
  if (list->children_num > 1) {
    free_t(pop_t(list, 0));
  }
  return list;
}

types *builtin_quote_t(types *args) {
  assert_t(args->children_num == 1, args, ERR_INVALID_ARGS,
           "too many arguments given to function 'quote'");
  types *quote = take_t(args, 0);
  return quote;
}