#ifndef env_h
#define env_h

#include "builtins.h"

struct env_t {
  int num_elements;
  char **symbols;
  val_t **values;
};

env_t *new_env_t();
void free_env_t(env_t *env);

val_t *env_get_t(env_t *env, val_t *symbol);
void env_add_t(env_t *env, val_t *key, val_t *val);

void env_add_builtin_t(env_t *env, char *name, builtin_t func);

#endif //env_h
