
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "types.c"
#include "blisp.c"
#include "parse.c"

#include "builtin/basic.c"
#include "builtin/arith.c"
#include "builtin/types.c"
#include "builtin/include.c"

int main(int argc, char **argv)
{
    Parser p = {0};

    Expr *env = env_create(nil);
    sym_assoc(env, make_sym("define"), make_sym("define"));
    sym_assoc(env, make_sym("quote"),  make_sym("quote"));
    sym_assoc(env, make_sym("lambda"), make_sym("lambda"));
    sym_assoc(env, make_sym("cond"),   make_sym("cond"));
    sym_assoc(env, make_sym("car"),    make_bin_func(builtin_car));
    sym_assoc(env, make_sym("cdr"),    make_bin_func(builtin_cdr));
    sym_assoc(env, make_sym("cons"),   make_bin_func(builtin_cons));
    sym_assoc(env, make_sym("eval"),   make_bin_func(builtin_eval));
    sym_assoc(env, make_sym("print"),  make_bin_func(builtin_print));
    sym_assoc(env, make_sym("include"),make_bin_func(builtin_include));
    
    sym_assoc(env, make_sym("nil?"),   make_bin_func(builtin_is_nil));
    sym_assoc(env, make_sym("num?"),   make_bin_func(builtin_is_num));
    sym_assoc(env, make_sym("sym?"),   make_bin_func(builtin_is_sym));
    sym_assoc(env, make_sym("list?"),  make_bin_func(builtin_is_list));

    sym_assoc(env, make_sym("sum"),    make_bin_func(builtin_sum));
    sym_assoc(env, make_sym("+"),      make_bin_func(builtin_sum));
    sym_assoc(env, make_sym("prod"),   make_bin_func(builtin_prod));
    sym_assoc(env, make_sym("*"),      make_bin_func(builtin_prod));
    sym_assoc(env, make_sym("-"),      make_bin_func(builtin_sub));
    sym_assoc(env, make_sym("/"),      make_bin_func(builtin_div));
    sym_assoc(env, make_sym("%"),      make_bin_func(builtin_rem));
    sym_assoc(env, make_sym("=="),     make_bin_func(builtin_eq));
    sym_assoc(env, make_sym("<"),      make_bin_func(builtin_less));

    sym_assoc(env, make_sym("env"),    env);
    sym_assoc(env, make_sym("brk"),    make_bin_func(builtin_break));

    bool repl_mode = false;
    char *filename = "test/default.ls";
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

            parser_init(&p, input);

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
