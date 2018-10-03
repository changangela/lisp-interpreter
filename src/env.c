#include "env.h"
#include "types.h"
#include <stdlib.h>

void env_init_builtins_t(env_t *env);

env_t *new_env_t() {
  env_t *env = malloc(sizeof(env_t));
  env->num_elements = 0;
  env->symbols = NULL;
  env->values = NULL;

  env_init_builtins_t(env);
  return env;
}

void free_env_t(env_t *env) {
  for (int i = 0; i < env->num_elements; ++i) {
    free(env->symbols[i]);
    free_t(env->values[i]);
  }

  free(env->symbols);
  free(env->values);
  free(env);
}

val_t *env_get_t(env_t *env, val_t *symbol) {
  for (int i = 0; i < env->num_elements; ++i) {
    if (strcmp(env->symbols[i], symbol->symbol) == 0) {
      return copy_t(env->values[i]);
    }
  }

  return new_err_t(ERR_UNBOUND_SYMBOL, symbol->symbol);
}

void env_update_or_insert_t(env_t *env, val_t *key, val_t *val) {
  for (int i = 0; i < env->num_elements; ++i) {
    if (strcmp(env->symbols[i], key->symbol) == 0) {
      free_t(env->values[i]);
      env->values[i] = copy_t(val);
      return;
    }
  }

  env->num_elements++;
  env->symbols = realloc(env->symbols, sizeof(char *) * env->num_elements);
  env->values = realloc(env->values, sizeof(val_t *) * env->num_elements);

  env->symbols[env->num_elements - 1] = malloc(strlen(key->symbol) + 1);
  strcpy(env->symbols[env->num_elements - 1], key->symbol);
  env->values[env->num_elements - 1] = copy_t(val);
}

void env_insert_t(env_t *env, val_t *key, val_t *val) {
  if (!env_exists_t(env, key)) {
    env->num_elements++;
    env->symbols = realloc(env->symbols, sizeof(char *) * env->num_elements);
    env->values = realloc(env->values, sizeof(val_t *) * env->num_elements);

    env->symbols[env->num_elements - 1] = malloc(strlen(key->symbol) + 1);
    strcpy(env->symbols[env->num_elements - 1], key->symbol);
    env->values[env->num_elements - 1] = copy_t(val);
  }
}

bool env_exists_t(env_t *env, val_t *key) {
  for (int i = 0; i < env->num_elements; ++i) {
    if (strcmp(env->symbols[i], key->symbol) == 0) {
      return true;
    }
  }
  return false;
}

void env_add_builtin_t(env_t *env, char *name, builtin_t func) {
  val_t *key = new_symbol_t(name);
  val_t *value = new_func_t(func);
  env_update_or_insert_t(env, key, value);
  free_t(key);
  free_t(value);
}

void env_init_builtins_t(env_t *env) {
  env_add_builtin_t(env, "+", builtin_add_t);
  env_add_builtin_t(env, "-", builtin_sub_t);
  env_add_builtin_t(env, "*", builtin_mult_t);
  env_add_builtin_t(env, "/", builtin_div_t);

  env_add_builtin_t(env, "append", builtin_append_t);
  env_add_builtin_t(env, "car", builtin_car_t);
  env_add_builtin_t(env, "cdr", builtin_cdr_t);
  env_add_builtin_t(env, "defvar", builtin_defvar_t);
  env_add_builtin_t(env, "eval", builtin_eval_t);
  env_add_builtin_t(env, "list", builtin_list_t);
  env_add_builtin_t(env, "quote", builtin_quote_t);
}