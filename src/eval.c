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
  if (t->children_num == 1 && t->children[0]->type != SYMBOL_T)
    return eval_t(env, t->children[0]);

  val_t *func = eval_t(env, pop_t(t, 0));
  if (func->type != FUNC_T) {
    free_t(t);
    if (func->type == ERR_T) {
      return func;
    } else {
      free(func);
      return new_err_t(ERR_NO_SYMBOL, "");
    }
  }

  val_t *result = func->func(env, t);

  free_t(func);

  return result;
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