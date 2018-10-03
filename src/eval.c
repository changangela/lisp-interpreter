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

  val_t *head = eval_t(env, pop_t(t, 0));

  switch (head->type) {
    case ERR_T: return head;
    case FUNC_T: {
      val_t *result = head->func(env, t);
      free(head);
      return result;
    }
    default: {
      val_t *err = new_err_t(ERR_TYPE_MISMATCH, type_name(SYMBOL_T), type_name(head->type));
      free(head);
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