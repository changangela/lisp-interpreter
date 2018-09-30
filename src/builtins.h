#ifndef builtins_h
#define builtins_h

typedef struct env_t env_t;
typedef struct val_t val_t;

typedef val_t *(*builtin_t)(env_t*, val_t*);

val_t *builtin_add_t(env_t *env, val_t *args);
val_t *builtin_sub_t(env_t *env, val_t *args);
val_t *builtin_mult_t(env_t *env, val_t *args);
val_t *builtin_div_t(env_t *env, val_t *args);

val_t *builtin_append_t(env_t *env, val_t *args);
val_t *builtin_car_t(env_t *env, val_t *args);
val_t *builtin_cdr_t(env_t *env, val_t *args);
val_t *builtin_eval_t(env_t *env, val_t *args);
val_t *builtin_list_t(env_t *env, val_t *args);
val_t *builtin_quote_t(env_t *env, val_t *args);

#endif //builtins_h
