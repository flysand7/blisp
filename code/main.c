
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
#include <signal.h>

#include "blisp.h"
#include "profiler.c"

#include "memory.c"
#include "wrap.c"
#include "parse.c"
#include "blisp.c"

static char const *signal_name(int signal) {
    switch(signal) {
        case SIGABRT: return "SIGABRT";
        case SIGFPE:  return "SIGFPE";
        case SIGILL:  return "SIGILL";
        case SIGINT:  return "SIGINT";
        case SIGSEGV: return "SIGSEGV";
        case SIGTERM: return "SIGTERM";
    }
    return "UNKNOWN";
}

static void abort_handler(int signal)
{
    __builtin_trap();
    printf("[%s]: Program has been terminated\n", signal_name(signal));
    exit(1);
}


int main(int argc, char **argv)
{
    spall_ctx = SpallInit("trace.spall", 1);
    unsigned char *buffer = malloc(1*1024*1024);
    spall_buffer = (SpallBuffer){
        .length = 1*1024*1024,
        .data = buffer,
    };

    SpallBufferInit(&spall_ctx, &spall_buffer);

    signal(SIGABRT, abort_handler);
    signal(SIGFPE, abort_handler);
    signal(SIGILL, abort_handler);
    signal(SIGINT, abort_handler);
    signal(SIGSEGV, abort_handler);
    signal(SIGTERM, abort_handler);

    Parser p = {0};

    Expr *env = env_default(make_nil());
    if(!run_file(env, "std/std.lsp")) {
        printf("[warning]: std/std.lsp wasn't loaded.");
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
    SpallBufferQuit(&spall_ctx, &spall_buffer);
    SpallQuit(&spall_ctx);
    return 0;
}
