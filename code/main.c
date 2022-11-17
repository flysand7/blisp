
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

#include "intern.c"
#include "memory.c"
#include "builtin.c"
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

static void signal_handler(int signal)
{
    printf("[%s]: Program has been terminated\n", signal_name(signal));
    abort();
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

    signal(SIGABRT, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGTERM, signal_handler);

    log_file = fopen("blisp.log", "wb");
    Parser p = {0};

    mem_init();
    interns_init();

    Expr env = env_default(expr_nil());
    if(!run_file(env, "std/std.lsp", NULL)) {
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
            Expr code;
            if(parse_root_expr(&p, &code)) {
                Expr result = eval(env, code);
                expr_print(stdout, result);
            }
        }
    }
    else {
        if(!setjmp(error_return_buf)) {
            if(!run_file(env, filename, NULL)) {
                printf("[error]: file %s isn't found\n", filename);
                exit(1);
            }
        }
    }
    SpallBufferQuit(&spall_ctx, &spall_buffer);
    SpallQuit(&spall_ctx);
    return 0;
}
