#include "repl.h"

void repl() {
  parser_init();

  env_t *env = new_env_t();

  while (true) {
    char *buffer = readline("$ ");
    add_history(buffer);
    parser_parse(env, buffer);
    free(buffer);
  }
  parser_cleanup();
}
