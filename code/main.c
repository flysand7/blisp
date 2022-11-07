
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <math.h>
#include <setjmp.h>

#include "blisp.h"

// #include "builtin/include.c"

#include "wrap.c"
#include "parse.c"
#include "blisp.c"

int main(int argc, char **argv)
{
    setbuf(stdout, 0);
    Parser p = {0};

    Expr *env = env_default(make_nil());
    if(!run_file(env, "lisp/std.lsp")) {
        printf("[warning]: lisp/std.lsp wasn't loaded.");
    }

    bool repl_mode = true;
    char *filename;
    if(argc == 2) {
        char *arg = argv[1];
        repl_mode = false;
        filename = arg;
    }
    if(repl_mode) {
        for(;;) {
            char input[128];
            if(setjmp(error_return_buf)) {
            }
            putchar('\n');
            putchar('>');
            gets_s(input, sizeof input);
            parser_init(&p, "repl@stdin", input);
            Expr *code = parse_root_expr(&p);
            if(code != nil) {
                Expr *result = eval(env, code);
                expr_print(result);
            }
        }
    }
    else {
        if(!setjmp(error_return_buf)) {
            if(!run_file(env, filename)) {
                printf("[error]: file %s isn't found\n", filename);
                exit(1);
            }
        }
    }

    fflush(stdout);
    return 0;
}
