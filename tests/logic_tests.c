#include "../src/parser.h"
#define _ERROR "_ERROR_"

const int buffer_size = 256;

void test_equals(char *left, char *right, int test_case_number) {
  fprintf(stderr, "test case %d: ", test_case_number);

  if ((strcmp(right, _ERROR) == 0 && strstr(left, "error"))
    || (strcmp(left, right) == 0)) {
    fprintf(stderr, "passed");
  } else {
    fprintf(stderr, "failed ('%s' != '%s')", left, right);
  }

  fprintf(stderr, "\n");
}

struct test {
  char input[buffer_size];
  char output[buffer_size];
};

const int NUM_TESTS = 13;

struct test tests[NUM_TESTS] = {
  // binop operations
  {"(+ 1 2 (* 2 3 (/ 21 3)) (- 4 5))", "44"},
  {"(/ 9 0)", _ERROR},
  {"(defvar li (list 1 2 3 4))", "li"},
  {"li", "(1 2 3 4)"},
  {"(append li (list 5 6 7 8) (list 9 10 11 12))",
    "(1 2 3 4 5 6 7 8 9 10 11 12)"},
  {"(append li 5)", _ERROR},
  {"(car li)", "1"},
  {"(cdr li)", "(2 3 4)"},
  {"(eval (list 'cdr (car '((quote (a b)) c))))", "(b)"}, // todo: this case should return 'b' not ('b')
  {"(defun mult_x (y) (* x y))", "mult_x"},
  {"(mult_x 5)", _ERROR},
  {"(defvar x 7)", "x"},
  {"(mult_x 5)", "35"},
};

void run_logic_tests() {
  parser_init();
  env_t *env = new_env_t();
  for (int i = 0; i < NUM_TESTS; ++i) {
    char *buffer = malloc(buffer_size);
    parser_parse(env, buffer, tests[i].input);
    test_equals(buffer, tests[i].output, i + 1);
    free(buffer);
  }
  parser_cleanup();
  free_env_t(env);
}