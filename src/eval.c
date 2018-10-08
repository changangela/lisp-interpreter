#include "eval.h"

val_t *eval_s_expr_t(env_t *env, val_t *t);

val_t *eval_t(env_t *env, val_t *t) {
  switch (t->type) {
    case SYMBOL_T: {
      val_t *val = env_get_t(env, t);
      free_t(t);
      return val;
    }
    case S_EXPR_T: return eval_s_expr_t(env, t);
    default: return t;
  }
}

val_t *eval_s_expr_t(env_t *env, val_t *t) {
  if (t->children_num == 0) return t;
  if (t->children_num == 1) return eval_t(env, t->children[0]);

  val_t *head = eval_t(env, pop_t(t, 0));

  switch (head->type) {
    case ERR_T: return head;
    case FUNC_T: {
      val_t *result = head->func(env, t);
      free_t(head);
      return result;
    }
    case LAMBDA_T: {
      val_t *result = eval_lambda_t(env, head, t);
      free_t(head);
      return result;
    }
    default: {
      val_t *err = new_err_t(ERR_TYPE_MISMATCH, type_name_t(SYMBOL_T),
        type_name_t(head->type));
      free_t(head);
      return err;
    }
  }
}

val_t *deep_eval_t(env_t *env, val_t *args) {
  for (int i = 0; i < args->children_num; ++i) {
    args->children[i] = eval_t(env, args->children[i]);
    if (args->children[i]->type == ERR_T) {
      return take_t(args, i);
    }
  }
  return args;
}

val_t *eval_lambda_t(env_t *env, val_t *lambda, val_t *args) {
  assert_t(lambda->formals->children_num == args->children_num, args,
    args->children_num < lambda->formals->children_num ? ERR_NOT_ENOUGH_ARGS
      : ERR_TOO_MANY_ARGS, lambda->name == NULL ? "lambda" : lambda->name);

  for (int i = 0; i < args->children_num; ++i) {
    args->children[i] = eval_t(env, args->children[i]);
    if (args->children[i]->type == ERR_T) {
      return take_t(args, i);
    }
  }

  for (int i = 0; i < lambda->formals->children_num; ++i) {
    env_insert_t(lambda->env, lambda->formals->children[i], args->children[i]);
  }

  free_t(args);

  lambda->env->parent = env;
  return eval_t(lambda->env, copy_t(lambda->body));
}