
# BLisp - a minimal LISP dialect

This project is a small experiment of mine to see how much you can do with lisp
on the lowest levels, that is making the interpreter as small as possible and
implementing most of Scheme's functionality within the language itself.

## Features

So far the interpreter has the following features built-into the interpreter:

- Primitive expressions: integers, floats, strings, builtin functions, pairs
- Special values: `nil`, `inf`, `nan`
- Checking type of the expression:
  - `int?`
  - `flt?`
  - `nil`
  - `sym`
  - `pair`
  - `func`
- Integer functions:
  - Bitwise logic: `int-bnot`, `int-bor`, `int-band`, `int-bxor`
  - Arithmetic: `int-neg`, `int-add`, `int-sub`, `int-mul`, `int-div`, `int-rem`
  - Comparison: `int-les?`, `int-grt?`, `int-eq?`, `int-neq?`, `int-leq?`, `int-geq?`
- Floating-point functions
  - Conversion to/from integers: `flt-from-int`, `flt-round`, `flt-trunc`, `flt-ceil`, `flt-floor`
  - Classification: `flt-inf?`, `flt-nan?`, `flt-normal?`
  - Arithmetic: `flt-neg`, `flt-add`, `flt-sub`, `flt-mul`, `flt-div`
  - Comparison: `flt-les?`, `flt-grt?`, `flt-eq?`, `flt-neq?`, `flt-leq?`, `flt-geq?`
- Symbol equality: `sym-eq?`
- Pair functions:
  - `car`
  - `cdr`
  - `cons`
- I/O:
  - `print-ch`
  - `input-ch`
  - `print`

The booleans are not implemented in this lisp dialect. Similarly to C, integers
are used instead. The zero value corresponds to `true`, and non-zero value to
`false`. The identifiers `true`, `false` aren't defined.

## Building & Running

To build the project clang required. On windows run via Visual Studio developer's command prompt.

```
$ git clone https://github.com/flysand7/blisp.git
$ cd blisp
$ bake
$ blisp
```

This should open up the REPL mode of the interpreter. You can write something
simple, like

```
> (print (int-mul (int-add 5 3) -2))
-16
```
