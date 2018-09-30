#ifndef eval_h
#define eval_h

#include "../lib/mpc.h"
#include "types.h"

#define assert_t(cond, args, err_code, err) \
        if (!(cond)) { free_t(args); return new_err_t(err_code, err); }

types *eval_t(types *t);

#endif //eval_h
