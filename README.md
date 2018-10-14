# lisp-interpreter

**lisp-interpreter** is a [Common lisp](https://lisp-lang.org/) interpreter written in C.

```lisp
$ (eval (list 'cdr (car '((quote (a b)) c))))
(b)
$ (defvar li (list 1 2 3 4))
li
$ li
(1 2 3 4)
$ (append li (list 5 6 7 8) (list 9 10 11 12))
(1 2 3 4 5 6 7 8 9 10 11 12)
$ (defvar x 7)
x
$ (defun mult_x (y) (* x y))
mult_x
$ (mult_x 5)
35
```

## Usage and installation

```bash
git clone http://github.com/changangela/lisp-interpreter
cd lisp-interpreter
cmake .
make
./lisp-interpreter
```
