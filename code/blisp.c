
struct Expr typedef Expr;
struct Pair typedef Pair;

typedef enum ExprKind {
    EXPR_NIL,
    EXPR_PAIR,
    EXPR_SYM,
    EXPR_NUM,
    EXPR_BIN_FUNC,
    EXPR_CLOSURE,
} ExprKind;

Expr typedef *(BinFunc)(Expr *args);

struct Expr {
    ExprKind kind;
    union {
        struct{ Expr *car, *cdr; };
        char    *str;
        i64      num;
        BinFunc *bin;
    };
};

struct Pair {
    Expr *car;
    Expr *cdr;
};

#define kind(e)        ((e) == nil ? EXPR_NIL : (e)->kind)

#define is_atom(e)     (kind(e) != EXPR_PAIR)
#define is_pair(e)     (kind(e) == EXPR_PAIR)
#define is_sym(e)      (kind(e) == EXPR_SYM)
#define is_num(e)      (kind(e) == EXPR_NUM)
#define is_nil(e)      (kind(e) == EXPR_NIL)
#define is_bin_func(e) (kind(e) == EXPR_BIN_FUNC)
#define is_closure(e)  (kind(e) == EXPR_CLOSURE)

#define val_num(e)  ((e)->num)
#define val_str(e)  ((e)->str)
#define sym_name(e) ((e)->str)
#define bin_func(e) ((e)->bin)

#define car(s) ((s)->car)
#define cdr(s) ((s)->cdr)
static Expr *cons(Expr *car_val, Expr *cdr_val) {
    Expr *pair = calloc(1, sizeof(Expr));
    pair->kind = EXPR_PAIR;
    car(pair) = car_val;
    cdr(pair) = cdr_val;
    return pair;
}

static bool sym_eq(Expr *sym1, Expr *sym2)
{
    assert(is_sym(sym1));
    assert(is_sym(sym2));
    char *name1 = sym_name(sym1);
    char *name2 = sym_name(sym2);
    return strcmp(name1, name2) == 0;
}

static bool sym_is(Expr *sym, char *name)
{
    assert(is_sym(sym));
    return strcmp(sym_name(sym), name) == 0;
}

static Expr *make_num(i64 value)
{
    Expr *num = calloc(1, sizeof(Expr));
    num->kind = EXPR_NUM;
    val_num(num) = value;
    return num;
}

static Expr *make_sym(char *name)
{
    Expr *sym = calloc(1, sizeof(Expr));
    sym->kind = EXPR_SYM;
    sym_name(sym) = name;
    return sym;
}

static Expr *make_bin_func(BinFunc *f)
{
    Expr *expr = calloc(1, sizeof(Expr));
    expr->kind = EXPR_BIN_FUNC;
    bin_func(expr) = f;
    return expr;
}

static bool is_list(Expr *expr)
{
    if(is_nil(expr)) {
        return true;
    }
    if(is_pair(expr)) {
        return is_list(cdr(expr));
    }
    return false;
}


static Expr *list_add(Expr *list, Expr *element)
{
    assert(is_list(list));
    if(is_nil(list)) {
        return cons(element, nil);
    }
    cdr(list) = list_add(cdr(list), element);
    return list;
}

static i64 listn(Expr *list)
{
    i64 count = 0;
    while(list != nil) {
        count += 1;
        list = cdr(list);
    }
    return count;
}

static bool is_quoted(Expr *expr)
{
    if(is_list(expr)) {
        Expr *fn = car(expr);
        if(is_sym(fn) && sym_is(fn, "quote")) {
            return true;
        }
    }
    return false;
}

static Expr *unquote(Expr *expr)
{
    return car(cdr(expr));
}

#define foreach(name, list) \
    for(Expr *name, *sublist = list;   \
        name = (is_nil(sublist)? nil : car(sublist)), !is_nil(sublist);  \
        sublist = cdr(sublist))

#define foreach_kv(kname, vname, list) \
    for(Expr *kname, *vname, *sublist = list;   \
        kname = (is_nil(sublist)? nil : car(sublist)), \
        vname = (is_nil(cdr(sublist))? nil : car(cdr(sublist))), \
        !is_nil(cdr(sublist));  \
        sublist = cdr(cdr(sublist)))

static void expr_print(Expr *expr)
{
    switch(kind(expr)) {
        case EXPR_NIL: printf("nil");                  break;
        case EXPR_SYM: printf("%s",   sym_name(expr)); break;
        case EXPR_NUM: printf("%lld", val_num(expr));  break;
        case EXPR_BIN_FUNC: printf("<fn: %p>", bin_func(expr)); break;
        case EXPR_PAIR: {
            Expr *sexp = expr;
            putchar('(');
            expr_print(car(sexp));
            sexp = cdr(sexp);
            while(!is_nil(sexp)) {
                if(sexp->kind == EXPR_PAIR) {
                    putchar(' ');
                    expr_print(car(sexp));
                    sexp = cdr(sexp);
                }
                else {
                    printf(" . ");
                    expr_print(sexp);
                    break;
                }
            }
            putchar(')');
        } break;
        case EXPR_CLOSURE: {
            printf("<closure>");
        } break;
    }
}

static void expr_println(Expr *expr)
{
    expr_print(expr);
    putchar('\n');
}


static Expr *eval(Expr *env, Expr *expr);
static Expr *apply(Expr *callsite_env, Expr *func, Expr *args);

// Example environment:
// (parent
//    (symbol1 . value1)
//    (symbol2 . value2)
//    ...
// )

#define parent(env)    car(env)
#define bindings(env)  cdr(env)
#define bind_sym(bind) car(bind)
#define bind_val(bind) cdr(bind)

static Expr *env_create(Expr *parent)
{
    return cons(parent, nil);
}

static Expr *sym_lookup(Expr *env, Expr *symbol)
{
    foreach(bind, bindings(env)) {
        if(sym_eq(symbol, bind_sym(bind))) {
            return bind_val(bind);
        }
    };

    if(is_nil(parent(env))) {
        return nil;
    }

    return sym_lookup(parent(env), symbol);
}

static void sym_assoc(Expr *env, Expr *symbol, Expr *value)
{
    bool found = false;
    foreach(bind, bindings(env)) {
        if(sym_eq(symbol, bind_sym(bind))) {
            bind_val(bind) = value;
            found = true;
            break;
        }
    };

    if(!found) {
        Expr *bind = cons(symbol, value);
        env = list_add(env, bind);
    }
}

// Closure example:
// (environment
//    ((x y z) . (body)))

#define closure_env(closure)    car(closure)
#define closure_params(closure) car(cdr(closure))
#define closure_body(closure)   cdr(cdr(closure))

static Expr *make_closure(Expr *env, Expr *params, Expr *body)
{
    assert(is_list(params));
    assert(is_list(body));

    foreach(param, params) {
        assert(is_sym(param)
           || (is_quoted(param) && is_sym(unquote(param))));
    }

    Expr *closure = cons(env, cons(params, body));
    closure->kind = EXPR_CLOSURE;
    return closure;
}


static bool is_literal(Expr *expr)
{
    return is_num(expr)
        || is_nil(expr)
        || is_bin_func(expr)
        || is_closure(expr);
}

static Expr *evlist(Expr *env, Expr *args)
{
    Expr *evargs = nil;
    foreach(arg, args) {
        Expr *evarg = eval(env, arg);
        evargs = list_add(evargs, evarg);
    }
    return evargs;
}

static void param_assoc(Expr *callsite_env, Expr *env, Expr *param, Expr *arg)
{
    if(!is_quoted(param)) {
        Expr *evarg;
        if(!sym_is(param, "...")) {
            evarg = eval(callsite_env, arg);
        }
        else {
            evarg = evlist(callsite_env, arg);
        }
        sym_assoc(env, param, evarg);
    }
    else {
        param = unquote(param);
        sym_assoc(env, param, arg);
    }
}

static void params_assoc(Expr *callsite_env, Expr *env, Expr *params, Expr *args)
{
    for(;;) {
        if(is_nil(args) && is_nil(params)) break;
        assert(!is_nil(args));
        assert(!is_nil(params));
        Expr *arg = car(args);
        Expr *param = car(params);
        
        {
            Expr *sym;
            if(is_quoted(param))
                sym = unquote(param);
            else
                sym = param;

            if(sym_is(sym, "...")) {
                arg = args;
                args = nil;
                param_assoc(callsite_env, env, param, arg);
                break;
            }
        }

        param_assoc(callsite_env, env, param, arg);

        args = cdr(args);
        params = cdr(params);
    }
}

static Expr *apply(Expr *callsite_env, Expr *func, Expr *args)
{
    if(is_bin_func(func)) {
        Expr *evargs = evlist(callsite_env, args);
        return bin_func(func)(evargs);
    }
    else if(is_closure(func)) {
        Expr *env    = closure_env(func);
        Expr *params = closure_params(func);
        Expr *body   = closure_body(func);
        params_assoc(callsite_env, env, params, args);
        Expr *result = nil;
        foreach(expr, body) {
            result = eval(env, expr);
        }
        return result;
    }
    else {
        assert(false);
        return nil;
    }
}

static Expr *eval(Expr *env, Expr *expr)
{
    // Literals are evaluated to themselves
    if(is_literal(expr)) {
        return expr;
    }

    // Symbols are evaluated to whatever they
    // are associated to in the environment.
    if(is_sym(expr)) {
        return sym_lookup(env, expr);
    }

    // (op arg1 arg2 ... argn)
    assert(is_list(expr));

    Expr *op   = car(expr);
    Expr *args = cdr(expr);

    assert(!is_nil(op));

    // Special forms, e.g quote and define
    if(is_sym(op)) {
        if(sym_is(op, "quote")) {
            assert(listn(args) == 1);
            Expr *arg = car(args);
            return arg;
        }
        else if(sym_is(op, "define")) {
            assert(listn(args) == 2);
            Expr *sym = car(args);
            Expr *value = eval(env, car(cdr(args)));
            if(!is_sym(sym)) {
                assert(false);
            }
            sym_assoc(env, sym, value);
            return value;
        }
        else if(sym_is(op, "lambda")) {
            assert(listn(args) >= 2);
            Expr *params = car(args);
            Expr *body = cdr(args);
            return make_closure(env, params, body);
        }
        else if(sym_is(op, "cond")) {
            foreach_kv(test, branch, args) {
                Expr *cond = eval(env, test);
                assert(is_num(cond));
                if(val_num(cond)) {
                    return eval(env, branch);
                }
            }
            return nil;
        }
        else if(sym_is(op, "do")) {
            Expr *last = nil;
            foreach(arg, args) {
                last = eval(env, arg);
            }
            return last;
        }
    }

    Expr *func = eval(env, op);
    return apply(env, func, args);
}
