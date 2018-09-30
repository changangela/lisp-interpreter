#ifndef parser_h
#define parser_h

#include "../lib/mpc.h"

mpc_parser_t *Number, *Symbol, *Expr, *Program, *S_expr, *Quote;

void parser_init();
void parser_parse(char *buffer);
void parser_cleanup();

#endif //parser_h
