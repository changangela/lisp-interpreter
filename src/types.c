#include "types.h"

types new_tnumber(long number) {
  types ret;
  ret.type = NUMBER_T;
  ret.number = number;
  return ret;
}

types new_terr(int err_code) {
  types ret;
  ret.type = ERR_T;
  ret.err = err_code;
  return ret;
}

bool is_terr(types t) {
  return t.type == ERR_T;
}

void tprint(types t) {
  switch (t.type) {
    case NUMBER_T: {
      printf("%li" , t.number);
      return;
    }
    case ERR_T: {
      switch (t.err) {
        case ERR_DIV_ZERO: {
          printf("error: divide by zero");
          return;
        }
        case ERR_INVALID_OP: {
          printf("error: invalid operation");
          return;
        }
        case ERR_INVALID_NUMBER: {
          printf("error: invalid number");
          return;
        }
      }
    }
  }
}

void tprintln(types t) {
  tprint(t);
  printf("\n");
}