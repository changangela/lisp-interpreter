#include "repl.h"

void repl() {
  parser_init();
  while (true) {
    char *buffer = readline("$ ");
    add_history(buffer);
    parser_parse(buffer);
    free(buffer);
  }
  parser_cleanup();
}
