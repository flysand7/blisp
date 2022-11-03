
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <math.h>

#include "blisp.h"

// #include "builtin/include.c"

#include "wrap.c"
#include "parse.c"
#include "blisp.c"

int main(int argc, char **argv)
{
    Parser p = {0};

    Expr *env = env_default(make_nil());

    bool repl_mode = false;
    char *filename = "lisp/default.lsp";
    if(argc == 2) {
        char *arg = argv[1];
        if(strcmp(arg, "/r") == 0) {
            repl_mode = true;
        }
        else {
            repl_mode = false;
            filename = arg;
        }
    }

    if(repl_mode) {
        for(;;) {
            char input[128];

            putchar('\n');
            putchar('>');
            gets_s(input, sizeof input);

            parser_init(&p, "repl@stdin", input);

            Expr *code = parse_expr(&p);
            Expr *result = eval(env, code);

            expr_print(result);
        }
    }
    else {
        run_file(env, filename);
    }

    return 0;
}
