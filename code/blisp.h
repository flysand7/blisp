
#if !defined(BLISP_H)
#define BLISP_H

uint8_t  typedef u8;
int8_t   typedef i8;
uint64_t typedef u64;
int64_t  typedef i64;
float    typedef f32;
double   typedef f64;

#define until(p) while(!(p))
#define repeat   do
#define nil      NULL

FILE *log_file = nil;

struct Alloc typedef Alloc;
struct Expr typedef Expr;
struct Pair typedef Pair;
struct Intern typedef Intern;

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
    bool     macro;
    union {
        struct{ Expr *car, *cdr; };
        Intern *str;
        i64   i;
        f64   f;
        Func *bin;
    };
};

struct Pair {
    Expr *car;
    Expr *cdr;
};

struct Alloc {
    Expr expr;
    bool mark;
    Alloc *next;
};

jmp_buf error_return_buf;

#define      kind(expr) (expr)->kind
#define      atom(expr) (expr)->atom

static Expr *new_expr(ExprKind kind, bool atomic);

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
static Expr *make_sym_intern(Intern *str);
#define      val_sym(e) ((e)->str)
static bool  sym_eq(Expr *sym1, Expr *sym2);
static bool  sym_is(Expr *sym, Intern *name);
#define      is_sym(e) (kind(e) == EXPR_SYM)

// Pairs
static Expr *cons(Expr *car_val, Expr *cdr_val);
#define      car(expr) (expr)->car
#define      cdr(expr) (expr)->cdr
#define      is_pair(e) (kind(e) == EXPR_PAIR)

// Lists
static Expr *list(int n, ...);
static void  list_plugb(Expr *list, Expr *value);
static void  list_pushb(Expr *list, Expr *element);
static void  list_reverse(Expr **list);
static i64   listn(Expr *list);
static Expr *list_ith(Expr *list, i64 i);
static void  list_set(Expr *list, i64 i, Expr *value);
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
#define      closure_env(closure)    car(cdr(closure))
#define      closure_params(closure) car(cdr(cdr(closure)))
#define      closure_body(closure)   car(cdr(cdr(cdr(closure))))

// Macro
static bool  is_macro(Expr *expr);
static Expr *make_macro(Expr *env, Expr *pars, Expr *body);
#define      macro_env(macro)        car(cdr(closure))
#define      macro_params(macro)     car(cdr(cdr(closure)))
#define      macro_body(macro)       car(cdr(cdr(cdr(closure))))

// Environment
static Expr *env_create(Expr *parent);
static Expr *env_default(Expr *parent);
static Expr *env_lookup(Expr *env, Expr *symbol);
static void  env_bind(Expr *env, Expr *symbol, Expr *value);
#define      env_parent(env)   car(env)
#define      env_bindings(env) cdr(env)
#define      bind_sym(bind)    car(bind)
#define      bind_val(bind)    cdr(bind)

// Stack frame
static Expr *make_frame(Expr *parent, Expr *env, Expr *op, Expr *args);
#define frame_parent(frame) car(frame)
#define frame_env(frame)    car(cdr(frame))
#define frame_op(frame)     car(cdr(cdr(frame)))
#define frame_arg(frame)    car(cdr(cdr(cdr(frame))))
#define frame_ev_op(frame)  car(cdr(cdr(cdr(cdr(frame)))))
#define frame_ev_arg(frame) car(cdr(cdr(cdr(cdr(cdr(frame))))))
#define frame_body(frame)   car(cdr(cdr(cdr(cdr(cdr(cdr(frame)))))))

// Evaluator
static Expr *eval(Expr *env, Expr *expr);

// Expression printing
static Expr *expr_print(FILE *file, Expr *expr);
static Expr *expr_println(FILE *file, Expr *expr);

#endif
