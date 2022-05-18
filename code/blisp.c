
static bool is_literal(Expr *expr)
{
    return is_int(expr)
        || is_flt(expr)
        || is_nil(expr)
        || is_func(expr)
        || is_closure(expr);
}

// Numbers

static Expr *make_int(i64 value)
{
    Expr *num = calloc(1, sizeof(Expr));
    num->kind = EXPR_INT;
    val_int(num) = value;
    return num;
}

// Floats

static Expr *make_flt(f64 value)
{
    Expr *num = calloc(1, sizeof(Expr));
    num->kind = EXPR_FLT;
    val_flt(num) = value;
    return num;
}

// Booleans

static Expr *make_bool(bool b)
{
    return make_int(b);
}

static bool val_bool(Expr *expr)
{
    return val_int(expr);
}

// Symbols

static Expr *make_sym(char *name)
{
    Expr *sym = calloc(1, sizeof(Expr));
    sym->kind = EXPR_SYM;
    val_sym(sym) = name;
    return sym;
}

static bool sym_eq(Expr *sym1, Expr *sym2)
{
    assert(is_sym(sym1) && is_sym(sym2));
    return strcmp(val_sym(sym1), val_sym(sym2)) == 0;
}

static bool sym_is(Expr *sym, char *name)
{
    assert(is_sym(sym));
    return strcmp(val_sym(sym), name) == 0;
}

// Pairs

static Expr *cons(Expr *car_val, Expr *cdr_val)
{
    Expr *pair = calloc(1, sizeof(Expr));
    pair->kind = EXPR_PAIR;
    pair->car = car_val;
    pair->cdr = cdr_val;
    return pair;
}

// Lists

static Expr *list(int n, ...)
{
    Expr *list = nil;
    va_list args;
    va_start(args, n);
    for(i64 i = 0; i != n; ++i) {
        Expr *element = va_arg(args, Expr *);
        list = list_pushb(list, element);
    }
    va_end(args);
    return list;
}

static Expr *list_append(Expr *list, Expr *element)
{
    assert(is_list(list));
    if(is_nil(list)) {
        return element;
    }
    list->cdr = list_append(cdr(list), element);
    return list;
}

static Expr *list_pushb(Expr *list, Expr *element)
{
    assert(is_list(list));
    return list_append(list, cons(element, nil));
}

static Expr *list_pushf(Expr *list, Expr *element)
{
    assert(is_list(list));
    return cons(element, list);
}

static i64 listn(Expr *list)
{
    i64 count = 0;
    while(!is_nil(list)) {
        count += 1;
        list = cdr(list);
    }
    return count;
}

static Expr *list_ith(Expr *list, i64 i)
{
    assert(0 <= i && i < listn(list));
    for(i64 k = 0; k != i; ++k) {
        list = cdr(list);
    }
    return car(list);
}

static bool is_list(Expr *expr)
{
    if(is_nil(expr))  return true;
    if(is_pair(expr)) return is_list(cdr(expr));
    return false;
}

// C functions

static Expr *make_func(Func *f)
{
    Expr *expr = calloc(1, sizeof(Expr));
    expr->kind = EXPR_FUNC;
    func(expr) = f;
    return expr;
}

// Closures

static Expr *make_closure(Expr *env, Expr *pars, Expr *body)
{
    assert(is_list(pars));
    assert(is_list(body));
    Expr *param_list = pars;
    foreach(param, param_list) {
        assert(is_sym(param));
    }
    Expr *closure = cons(env, cons(pars, body));
    closure->kind = EXPR_CLOSURE;
    return closure;
}

// Environment

static Expr *env_create(Expr *parent)
{
    return cons(parent, nil);
}

static Expr *env_default(Expr *parent)
{
    Expr *env = env_create(parent);
    env_assoc_sym(env, make_sym("int?"),        make_func(f_is_int));
    env_assoc_sym(env, make_sym("flt?"),        make_func(f_is_flt));
    env_assoc_sym(env, make_sym("nil?"),        make_func(f_is_nil));
    env_assoc_sym(env, make_sym("sym?"),        make_func(f_is_sym));
    env_assoc_sym(env, make_sym("pair?"),       make_func(f_is_pair));
    env_assoc_sym(env, make_sym("list?"),       make_func(f_is_list));
    env_assoc_sym(env, make_sym("func?"),       make_func(f_is_func));
    env_assoc_sym(env, make_sym("closure?"),    make_func(f_is_closure));

    // Integers
    env_assoc_sym(env, make_sym("int-bnot"),    make_func(f_int_bnot));
    env_assoc_sym(env, make_sym("int-neg"),     make_func(f_int_neg));
    env_assoc_sym(env, make_sym("int-add"),     make_func(f_int_add));
    env_assoc_sym(env, make_sym("int-sub"),     make_func(f_int_sub));
    env_assoc_sym(env, make_sym("int-mul"),     make_func(f_int_mul));
    env_assoc_sym(env, make_sym("int-div"),     make_func(f_int_div));
    env_assoc_sym(env, make_sym("int-rem"),     make_func(f_int_rem));
    env_assoc_sym(env, make_sym("int-les"),     make_func(f_int_les));
    env_assoc_sym(env, make_sym("int-grt"),     make_func(f_int_grt));
    env_assoc_sym(env, make_sym("int-eq"),      make_func(f_int_eq));
    env_assoc_sym(env, make_sym("int-neq"),     make_func(f_int_neq));
    env_assoc_sym(env, make_sym("int-leq"),     make_func(f_int_leq));
    env_assoc_sym(env, make_sym("int-geq"),     make_func(f_int_geq));
    env_assoc_sym(env, make_sym("int-band"),    make_func(f_int_band));
    env_assoc_sym(env, make_sym("int-bor"),     make_func(f_int_bor));
    env_assoc_sym(env, make_sym("int-bxor"),    make_func(f_int_bxor));

    // Floats
    env_assoc_sym(env, make_sym("flt-from-int"),make_func(f_flt_from_int));
    env_assoc_sym(env, make_sym("flt-floor"),   make_func(f_flt_floor));
    env_assoc_sym(env, make_sym("flt-ceil"),    make_func(f_flt_ceil));
    env_assoc_sym(env, make_sym("flt-round"),   make_func(f_flt_round));
    env_assoc_sym(env, make_sym("flt-trunc"),   make_func(f_flt_trunc));
    env_assoc_sym(env, make_sym("flt-neg"),     make_func(f_flt_neg));
    env_assoc_sym(env, make_sym("flt-inf?"),    make_func(f_flt_is_inf));
    env_assoc_sym(env, make_sym("flt-nan?"),    make_func(f_flt_is_nan));
    env_assoc_sym(env, make_sym("flt-normal?"), make_func(f_flt_is_normal));
    env_assoc_sym(env, make_sym("flt-add"),     make_func(f_flt_add));
    env_assoc_sym(env, make_sym("flt-sub"),     make_func(f_flt_sub));
    env_assoc_sym(env, make_sym("flt-mul"),     make_func(f_flt_mul));
    env_assoc_sym(env, make_sym("flt-div"),     make_func(f_flt_div));
    env_assoc_sym(env, make_sym("flt-les"),     make_func(f_flt_les));
    env_assoc_sym(env, make_sym("flt-grt"),     make_func(f_flt_grt));
    env_assoc_sym(env, make_sym("flt-eq"),      make_func(f_flt_eq));
    env_assoc_sym(env, make_sym("flt-neq"),     make_func(f_flt_neq));
    env_assoc_sym(env, make_sym("flt-leq"),     make_func(f_flt_leq));
    env_assoc_sym(env, make_sym("flt-geq"),     make_func(f_flt_geq));

    // Pairs
    env_assoc_sym(env, make_sym("car"),         make_func(f_car));
    env_assoc_sym(env, make_sym("cdr"),         make_func(f_cdr));
    env_assoc_sym(env, make_sym("cons"),        make_func(f_cons));

    env_assoc_sym(env, make_sym("eval"),        make_func(f_eval));
    env_assoc_sym(env, make_sym("apply"),       make_func(f_apply));
    env_assoc_sym(env, make_sym("print"),       make_func(f_print));
    env_assoc_sym(env, make_sym("inc"),         make_func(include));
    env_assoc_sym(env, make_sym("env"),         env);
    return env;
}

static Expr *env_lookup(Expr *env, Expr *symbol)
{
    Expr *bindings = env_bindings(env);
    foreach(bind, bindings) {
        if(sym_eq(symbol, bind_sym(bind))) {
            return bind_val(bind);
        }
    };
    if(is_nil(env_parent(env))) return nil;
    return env_lookup(env_parent(env), symbol);
}

static void env_assoc_sym(Expr *env, Expr *symbol, Expr *value)
{
    bool found = false;
    Expr *bindings = env_bindings(env);
    foreach(bind, bindings) {
        if(sym_eq(symbol, bind_sym(bind))) {
            bind_val(bind) = value;
            found = true;
            break;
        }
    };
    if(!found) {
        Expr *bind = cons(symbol, value);
        env = list_pushb(env, bind);
    }
}

static Expr *apply(Expr *op, Expr *args)
{
    if(is_func(op)) {
        return func(op)(args);
    }
    else if(is_closure(op)) {
        Expr *env  = closure_env(op);
        Expr *pars = closure_params(op);
        Expr *body = closure_body(op);

        foreach2(par, pars, arg, args) {
            env_assoc_sym(env, par, arg);
        }
        assert(is_nil(args) && is_nil(pars));

        Expr *result = nil;
        Expr *expr_list = body;
        foreach(expr, expr_list) {
            result = eval(env, expr);
        }
        return result;
    }
    else {
        assert(false);
        return nil;
    }
}

static Expr *evlist(Expr *env, Expr *arg_list)
{
    Expr *evargs = nil;
    Expr *args = arg_list;
    foreach(arg, args) {
        Expr *evarg = eval(env, arg);
        evargs = list_pushb(evargs, evarg);
    }
    return evargs;
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
        return env_lookup(env, expr);
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
        else if(sym_is(op, "def")) {
            assert(listn(args) == 2);
            Expr *sym = car(args);
            Expr *value = eval(env, car(cdr(args)));
            if(!is_sym(sym)) {
                assert(false);
            }
            env_assoc_sym(env, sym, value);
            return value;
        }
        else if(sym_is(op, "lambda")) {
            assert(listn(args) >= 2);
            Expr *new_env = env_create(env);
            Expr *params = car(args);
            Expr *body = cdr(args);
            return make_closure(new_env, params, body);
        }
        else if(sym_is(op, "cond")) {
            Expr *cases = args;
            foreach_kv(test, branch, cases) {
                Expr *cond = eval(env, test);
                assert(is_int(cond));
                if(val_int(cond)) {
                    return eval(env, branch);
                }
            }
            assert(is_nil(cases));
            return nil;
        }
        else if(sym_is(op, "do")) {
            Expr *last = nil;
            Expr *expr_list = args;
            foreach(expr, expr_list) {
                last = eval(env, expr);
            }
            return last;
        }
    }

    Expr *func = eval(env, op);
    Expr *evargs = evlist(env, args);
    return apply(func, evargs);
}

// Expression printing

static Expr *expr_print(Expr *expr)
{
    switch(kind(expr)) {
        case EXPR_NIL: printf("nil");                  break;
        case EXPR_SYM: printf("%s",   val_sym(expr));  break;
        case EXPR_INT: printf("%lld", val_int(expr));  break;
        case EXPR_FLT: printf("%f",   val_flt(expr));  break;
        case EXPR_FUNC: printf("<fn: %p>", func(expr)); break;
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
    return expr;
}

static Expr *expr_println(Expr *expr)
{
    expr_print(expr);
    putchar('\n');
    return expr;
}
