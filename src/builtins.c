#include "builtins.h"
#include "types.h"
#include "eval.h"

val_t *builtin_binop_t(env_t *env, char *op, val_t *args);

val_t *builtin_add_t(env_t *env, val_t *args) {
  return builtin_binop_t(env, "+", args);
}

val_t *builtin_sub_t(env_t *env, val_t *args) {
  return builtin_binop_t(env, "-", args);
}

val_t *builtin_mult_t(env_t *env, val_t *args) {
  return builtin_binop_t(env, "*", args);
}

val_t *builtin_div_t(env_t *env, val_t *args) {
  return builtin_binop_t(env, "/", args);
}

val_t *builtin_list_t(env_t *env, val_t *args) {
  args = deep_eval_t(env, args);
  if (args->type == ERR_T) {
    return args;
  }

  return args;
}

val_t *builtin_car_t(env_t *env, val_t *args) {
  args = deep_eval_t(env, args);
  if (args->type == ERR_T) {
    return args;
  }

  assert_t(args->children_num == 1, args, ERR_TOO_MANY_ARGS, "car");
  assert_t(args->children[0]->type == S_EXPR_T, args, ERR_INVALID_LIST_ARG,
    "car");
  assert_t(args->children[0]->children_num >= 1, args, ERR_INVALID_ARGS,
    "empty list passed to function 'car'");

  val_t *head = pop_t(args->children[0], 0);
  free_t(args);
  return head;
}

val_t *builtin_cdr_t(env_t *env, val_t *args) {
  args = deep_eval_t(env, args);
  if (args->type == ERR_T) {
    return args;
  }

  assert_t(args->children_num == 1, args, ERR_TOO_MANY_ARGS, "cdr");
  assert_t(args->children[0]->type == S_EXPR_T, args, ERR_INVALID_LIST_ARG,
    "cdr");

  val_t *list = take_t(args, 0);
  if (list->children_num > 1) {
    free_t(pop_t(list, 0));
  }
  return list;
}

val_t *builtin_quote_t(env_t *env, val_t *args) {
  assert_t(args->children_num == 1, args, ERR_INVALID_ARGS,
    "too many arguments given to function 'quote'");
  val_t *quote = take_t(args, 0);
  return quote;
}

val_t *builtin_eval_t(env_t *env, val_t *args) {
  args = deep_eval_t(env, args);
  if (args->type == ERR_T) {
    return args;
  }

  assert_t(args->children_num == 1, args, ERR_TOO_MANY_ARGS, "eval");
  assert_t(args->children[0]->type == S_EXPR_T, args, ERR_INVALID_LIST_ARG,
    "eval");
  val_t *expr = take_t(args, 0);
  return eval_s_expr_t(env, expr);
}

val_t *builtin_append_t(env_t *env, val_t *args) {
  args = deep_eval_t(env, args);
  if (args->type == ERR_T) {
    return args;
  }

  for (int i = 0; i < args->children_num; ++i) {
    assert_t(args->children[i]->type == S_EXPR_T, args, ERR_INVALID_LIST_ARG,
      "append");
  }

  val_t *head = pop_t(args, 0);
  while (args->children_num > 0) {
    val_t *current = pop_t(args, 0);
    join_t(head, current);
  }

  free_t(args);
  return head;
}

val_t *builtin_binop_t(env_t *env, char *op, val_t *args) {
  args = deep_eval_t(env, args);
  if (args->type == ERR_T) {
    return args;
  }

  for (int i = 0; i < args->children_num; ++i) {
    assert_t(args->children[i]->type == NUMBER_T, args, ERR_INVALID_NUMBER_ARG,
      op);
  }

  val_t *head = args->children_num > 0 ? pop_t(args, 0) : new_number_t(0);

  if (strcmp(op, "-") == 0 && args->children_num == 0) {
    head->number = -head->number;
  }

  while (args->children_num > 0) {
    val_t *current = pop_t(args, 0);
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