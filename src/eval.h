#ifndef eval_h
#define eval_h

#include "../lib/mpc.h"
#include "types.h"
#include "env.h"

#define assert_t(cond, args, err_code, err) if (!(cond)) { free_t(args); return new_err_t(err_code, err); }

val_t *eval_t(env_t *env, val_t *t);
val_t *eval_s_expr_t(env_t *env, val_t *t);
val_t *deep_eval_t(env_t *env, val_t *args);

#endif //eval_h
