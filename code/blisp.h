
#if !defined(BLISP_H)
#define BLISP_H

#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdio.h>

FILE *log_file = NULL;

struct Alloc typedef Alloc;
struct Expr typedef Expr;
struct Pair typedef Pair;
struct Intern typedef Intern;

typedef enum ExprKind {
    EXPR_NIL,
    EXPR_PAIR,
    EXPR_SYMBOL,
    EXPR_STRING,
    EXPR_INTEGER,
    EXPR_FLOAT,
    EXPR_FUNCTION,
    EXPR_CLOSURE,
    EXPR_MACRO,
} ExprKind;

Expr typedef (Func)(Expr args);

struct Expr {
    ExprKind kind;
    union {
        Pair   *pair;
        Intern *val_str;
        int64_t val_int;
        double  val_flt;
        Func   *func;
    };
};

struct Pair {
    Expr car;
    Expr cdr;
};

struct Alloc {
    Pair pair;
    bool mark;
    Alloc *next;
};

jmp_buf error_return_buf;

#define      kind(expr) (expr).kind
#define      atom(expr) (expr).atom

static Expr *new_expr(ExprKind kind, bool atomic);

// Nil
#define      is_nil(e) (kind(e) == EXPR_NIL)
static Expr  expr_nil();

// Numbers
static Expr  expr_int(int64_t value);
static Expr  expr_flt(double value);
#define      val_int(e) ((e).val_int)
#define      val_flt(e) ((e).val_flt)
#define      is_int(e) (kind(e) == EXPR_INTEGER)
#define      is_flt(e) (kind(e) == EXPR_FLOAT)

// Booleans
static Expr  expr_bool(bool b);
static bool  val_bool(Expr expr);

// Interns
// #define      intern_next(i) car(i)
// #define      intern_str(i)  car(cdr(i))

// Strings
static Expr  expr_str(char *str);
#define      val_str(e) ((e).val_str)
#define      is_str(e) (kind(e) == EXPR_STRING)

// Symbols
static Expr  expr_sym(char *name);
static Expr  expr_sym_intern(Intern *str);
#define      val_sym(e) ((e).val_str)
static bool  sym_eq(Expr sym1, Expr sym2);
static bool  sym_is(Expr sym, Intern *name);
#define      is_sym(e) (kind(e) == EXPR_SYMBOL)

// Pairs
static Expr cons(Expr car_val, Expr cdr_val);
#define     car(expr) (expr).pair->car
#define     cdr(expr) (expr).pair->cdr
#define     is_pair(e) (kind(e) == EXPR_PAIR || kind(e) == EXPR_CLOSURE || kind(e) == EXPR_MACRO)

// Lists
static void  list_reverse(Expr *list);
static Expr  list_ith(Expr list, int i);
static void  list_set(Expr list, int i, Expr value);

// C functions
static Expr  make_func(Func *f);
#define      func(e) ((e).func)
#define      is_func(e) (kind(e) == EXPR_FUNCTION)

// Closure
static bool is_closure(Expr expr);
static Expr make_closure(Expr env, Expr pars, Expr body);
#define     closure_env(closure)    car(closure)
#define     closure_params(closure) car(cdr(closure))
#define     closure_body(closure)   car(cdr(cdr(closure)))

// Macro
static bool is_macro(Expr expr);
static Expr make_macro(Expr env, Expr pars, Expr body);
#define     macro_env(macro)        car(closure)
#define     macro_params(macro)     car(cdr(closure))
#define     macro_body(macro)       car(cdr(cdr(closure)))

// Environment
static Expr env_create(Expr parent);
static Expr env_default(Expr parent);
static bool env_lookup(Expr env, Expr symbol, Expr *value);
static void env_bind(Expr env, Expr symbol, Expr value);
#define     env_parent(env) car(env)
#define     env_bindings(env) cdr(env)
#define     bind_sym(bind)    car(bind)
#define     bind_val(bind)    cdr(bind)

// Stack frame
static Expr make_frame(Expr parent, Expr env, Expr op, Expr args);
#define frame_parent(frame) car(frame)
#define frame_env(frame)    car(cdr(frame))
#define frame_op(frame)     car(cdr(cdr(frame)))
#define frame_arg(frame)    car(cdr(cdr(cdr(frame))))
#define frame_ev_op(frame)  car(cdr(cdr(cdr(cdr(frame)))))
#define frame_ev_arg(frame) car(cdr(cdr(cdr(cdr(cdr(frame))))))
#define frame_body(frame)   car(cdr(cdr(cdr(cdr(cdr(cdr(frame)))))))

// Evaluator
static Expr eval(Expr env, Expr expr);

// Expression printing
static Expr expr_print(FILE *file, Expr expr);
static Expr expr_println(FILE *file, Expr expr);

#endif
