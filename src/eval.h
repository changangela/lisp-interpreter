#ifndef eval_h
#define eval_h

#include "../lib/mpc.h"
#include "types.h"
#include "env.h"

#define assert_t(cond, args, err_code, ...) \
if (!(cond)) { \
  val_t* err = new_err_t(err_code, ##__VA_ARGS__); \
  free_t(args); \
  return err; \
}

val_t *eval_t(env_t *env, val_t *t);
val_t *eval_s_expr_t(env_t *env, val_t *t);
val_t *deep_eval_t(env_t *env, val_t *args);
val_t *eval_lambda_t(env_t *env, val_t *lambda, val_t *args);

#endif //eval_h
