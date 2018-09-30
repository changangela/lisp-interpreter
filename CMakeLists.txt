cmake_minimum_required(VERSION 3.12)
project(lisp_interpreter)

set(CMAKE_C_STANDARD 99)
add_executable(lisp_interpreter src/main.c lib/mpc.c lib/mpc.h src/parser.c src/parser.h src/repl.h src/repl.c src/eval.c src/eval.h src/types.c src/types.h src/builtins.c src/builtins.h src/env.c src/env.h)
target_link_libraries(lisp_interpreter /usr/lib/libedit.dylib)
