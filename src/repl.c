#include "repl.h"

void repl() {
  parser_init();
  env_t *env = new_env_t();
  const int buffer_size = 256;
  while (true) {
    char *buffer_in = readline("$ ");
    add_history(buffer_in);

    char *buffer_out = malloc(buffer_size);
    parser_parse(env, buffer_out, buffer_in);
    printf("%s\n", buffer_out);

    free(buffer_out);
    free(buffer_in);
  }

  parser_cleanup();
  free_env_t(env);
}
