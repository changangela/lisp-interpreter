#ifndef parser_h
#define parser_h

#include "../lib/mpc.h"

mpc_parser_t *Number, *Op, *Expr, *Program;

void parser_init();
void parser_parse();
void parser_cleanup();


#endif //parser_h
