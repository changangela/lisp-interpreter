#ifndef types_h
#define types_h

#include <stdio.h>
#include <stdbool.h>

enum { NUMBER_T, ERR_T };
enum { ERR_DIV_ZERO, ERR_INVALID_OP, ERR_INVALID_NUMBER };

typedef struct {
  int type;
  long number;
  int err;
} types;

types new_tnumber(long number);
types new_terr(int err_code);
bool is_terr(types t);
void tprint(types t);
void tprintln(types t);

#endif //types_h
