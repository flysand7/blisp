
#if !defined(BLISP_H)
#define BLISP_H

uint64_t typedef u64;
int64_t  typedef i64;
float    typedef f32;
double   typedef f64;

#define until(p) while(!(p))
#define repeat   do
#define nil      NULL

struct Expr typedef Expr;
struct Pair typedef Pair;

typedef enum ExprKind {
    EXPR_NIL,
    EXPR_PAIR,
    EXPR_SYM,
    EXPR_STR,
    EXPR_INT,
    EXPR_FLT,
    EXPR_FUNC,
} ExprKind;

Expr typedef *(Func)(Expr *args);

struct Expr {
    ExprKind kind;
    bool     atom;
    union {
        struct{ Expr *car, *cdr; };
        char *str;
        i64   i;
        f64   f;
        Func *bin;
    };
};

struct Pair {
    Expr *car;
    Expr *cdr;
};

#define      kind(expr) (expr)->kind
#define      atom(expr) (expr)->atom

// Nil
#define      is_nil(e) (kind(e) == EXPR_NIL)
static Expr *make_nil();

// Numbers
static Expr *make_int(i64 value);
static Expr *make_flt(f64 value);
#define      val_int(e) ((e)->i)
#define      val_flt(e) ((e)->f)
#define      is_int(e) (kind(e) == EXPR_INT)
#define      is_flt(e) (kind(e) == EXPR_FLT)

// Booleans
static Expr *make_bool(bool b);
static bool  val_bool(Expr *expr);

// Interns
#define      intern_next(i) car(i)
#define      intern_str(i)  car(cdr(i))

// Strings
static Expr *make_str(char *str);
#define      val_str(e) ((e)->str)
#define      is_str(e) (kind(e) == EXPR_STR)

// Symbols
static Expr *make_sym(char *name);
#define      val_sym(e) ((e)->str)
static bool  sym_eq(Expr *sym1, Expr *sym2);
static bool  sym_is(Expr *sym, char *name);
#define      is_sym(e) (kind(e) == EXPR_SYM)

// Pairs
static Expr *cons(Expr *car_val, Expr *cdr_val);
#define      car(expr) expr->car
#define      cdr(expr) expr->cdr
#define      is_pair(e) (kind(e) == EXPR_PAIR)

// Lists
static Expr *list(int n, ...);
static void  list_plugb(Expr *list, Expr *value);
static void  list_pushb(Expr *list, Expr *element);
static i64   listn(Expr *list);
static Expr *list_ith(Expr *list, i64 i);
static bool  is_list(Expr *expr);

#define foreach(name, list)                                     \
    for(Expr *name;                                             \
        name = (is_nil(list)? nil : car(list)), !is_nil(list);  \
        list = cdr(list))

// C functions
static Expr *make_func(Func *f);
#define      func(e) ((e)->bin)
#define      is_func(e) (kind(e) == EXPR_FUNC)

// Closure
static bool  is_closure(Expr *expr);
static Expr *make_closure(Expr *env, Expr *pars, Expr *body);
#define      closure_env(closure)    list_ith(closure, 1)
#define      closure_params(closure) list_ith(closure, 2)
#define      closure_body(closure)   list_ith(closure, 3)

// Environment
static Expr *env_create(Expr *parent);
static Expr *env_default(Expr *parent);
static Expr *env_lookup(Expr *env, Expr *symbol);
static void  env_bind(Expr *env, Expr *symbol, Expr *value);
#define      env_parent(env)   car(env)
#define      env_bindings(env) cdr(env)
#define      bind_sym(bind)    car(bind)
#define      bind_val(bind)    cdr(bind)

// Evaluator
static Expr *eval(Expr *env, Expr *expr);
static Expr *apply(Expr *op, Expr *pars);

// Expression printing
static Expr *expr_print(Expr *expr);
static Expr *expr_println(Expr *expr);

#endif
