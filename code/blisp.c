
static void expr_to_atom(Expr *expr)
{
    atom(expr) = true;
}

static Expr *new_expr(ExprKind kind, bool atomic)
{
    Expr *expr = calloc(1, sizeof(Expr));
    kind(expr) = kind;
    atom(expr) = atomic;
    return expr;
}

// Nil

static Expr *make_nil()
{
    return new_expr(EXPR_NIL, true);
}

// Numbers

static Expr *make_int(i64 value)
{
    Expr *expr = new_expr(EXPR_INT, true);
    val_int(expr) = value;
    return expr;
}

// Floats

static Expr *make_flt(f64 value)
{
    Expr *expr = new_expr(EXPR_FLT, true);
    val_flt(expr) = value;
    return expr;
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

// Strings

static Expr *make_str(char *str)
{
    Expr *expr = new_expr(EXPR_STR, true);
    val_str(expr) = str;
    return expr;
}

// Symbols

static Expr *make_sym(char *name)
{
    Expr *expr = new_expr(EXPR_SYM, false);
    val_sym(expr) = name;
    return expr;
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
    Expr *expr = new_expr(EXPR_PAIR, false);
    car(expr) = car_val;
    cdr(expr) = cdr_val;
    return expr;
}

// Lists

static Expr *list(int n, ...)
{
    Expr *list = make_nil();
    va_list args;
    va_start(args, n);
    for(i64 i = 0; i != n; ++i) {
        Expr *element = va_arg(args, Expr *);
        list_pushb(list, element);
    }
    va_end(args);
    return list;
}

static void list_plugb(Expr *list, Expr *value)
{
    if(is_nil(list))
        *list = *value; // fuck i don't like this
    else if(is_nil(cdr(list)))
        cdr(list) = value;
    else
        list_plugb(cdr(list), value);
}

static void list_pushb(Expr *list, Expr *element)
{
    assert(is_list(list));
    list_plugb(list, cons(element, make_nil()));
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
    Expr *expr = new_expr(EXPR_FUNC, true);
    func(expr) = f;
    return expr;
}

// Closures

static bool is_closure(Expr *expr)
{
    if(is_list(expr) && !is_nil(expr)) {
        Expr *flag = list_ith(expr, 0);
        if(is_sym(flag) && sym_is(flag, "closure")) {
            return true;
        }
    }
    return false;
}

static Expr *make_closure(Expr *env, Expr *pars, Expr *body)
{
    assert(is_list(body));
    Expr *closure = list(4, make_sym("closure"), env, pars, body);
    expr_to_atom(closure);
    return closure;
}

// Environment

static Expr *env_create(Expr *parent)
{
    return cons(parent, make_nil());
}

static Expr *env_default(Expr *parent)
{
    Expr *env = env_create(parent);

    env_bind(env, make_sym("nil"),         make_nil());

    env_bind(env, make_sym("int?"),        make_func(f_is_int));
    env_bind(env, make_sym("flt?"),        make_func(f_is_flt));
    env_bind(env, make_sym("nil?"),        make_func(f_is_nil));
    env_bind(env, make_sym("sym?"),        make_func(f_is_sym));
    env_bind(env, make_sym("pair?"),       make_func(f_is_pair));
    env_bind(env, make_sym("list?"),       make_func(f_is_list));
    env_bind(env, make_sym("func?"),       make_func(f_is_func));

    // Integers
    env_bind(env, make_sym("int-bnot"),    make_func(f_int_bnot));
    env_bind(env, make_sym("int-neg"),     make_func(f_int_neg));
    env_bind(env, make_sym("int-add"),     make_func(f_int_add));
    env_bind(env, make_sym("int-sub"),     make_func(f_int_sub));
    env_bind(env, make_sym("int-mul"),     make_func(f_int_mul));
    env_bind(env, make_sym("int-div"),     make_func(f_int_div));
    env_bind(env, make_sym("int-rem"),     make_func(f_int_rem));
    env_bind(env, make_sym("int-les?"),    make_func(f_int_les));
    env_bind(env, make_sym("int-grt?"),    make_func(f_int_grt));
    env_bind(env, make_sym("int-eq?"),     make_func(f_int_eq));
    env_bind(env, make_sym("int-neq?"),    make_func(f_int_neq));
    env_bind(env, make_sym("int-leq?"),    make_func(f_int_leq));
    env_bind(env, make_sym("int-geq?"),    make_func(f_int_geq));
    env_bind(env, make_sym("int-band"),    make_func(f_int_band));
    env_bind(env, make_sym("int-bor"),     make_func(f_int_bor));
    env_bind(env, make_sym("int-bxor"),    make_func(f_int_bxor));

    // Floats
    env_bind(env, make_sym("flt-from-int"),make_func(f_flt_from_int));
    env_bind(env, make_sym("flt-floor"),   make_func(f_flt_floor));
    env_bind(env, make_sym("flt-ceil"),    make_func(f_flt_ceil));
    env_bind(env, make_sym("flt-round"),   make_func(f_flt_round));
    env_bind(env, make_sym("flt-trunc"),   make_func(f_flt_trunc));
    env_bind(env, make_sym("flt-neg"),     make_func(f_flt_neg));
    env_bind(env, make_sym("flt-inf?"),    make_func(f_flt_is_inf));
    env_bind(env, make_sym("flt-nan?"),    make_func(f_flt_is_nan));
    env_bind(env, make_sym("flt-normal?"), make_func(f_flt_is_normal));
    env_bind(env, make_sym("flt-add"),     make_func(f_flt_add));
    env_bind(env, make_sym("flt-sub"),     make_func(f_flt_sub));
    env_bind(env, make_sym("flt-mul"),     make_func(f_flt_mul));
    env_bind(env, make_sym("flt-div"),     make_func(f_flt_div));
    env_bind(env, make_sym("flt-les?"),    make_func(f_flt_les));
    env_bind(env, make_sym("flt-grt?"),    make_func(f_flt_grt));
    env_bind(env, make_sym("flt-eq?"),     make_func(f_flt_eq));
    env_bind(env, make_sym("flt-neq?"),    make_func(f_flt_neq));
    env_bind(env, make_sym("flt-leq?"),    make_func(f_flt_leq));
    env_bind(env, make_sym("flt-geq?"),    make_func(f_flt_geq));

    // Symbols
    env_bind(env, make_sym("sym-eq?"),     make_func(f_is_sym_eq));

    // Pairs
    env_bind(env, make_sym("car"),         make_func(f_car));
    env_bind(env, make_sym("cdr"),         make_func(f_cdr));
    env_bind(env, make_sym("cons"),        make_func(f_cons));

    env_bind(env, make_sym("eval"),        make_func(f_eval));
    env_bind(env, make_sym("apply"),       make_func(f_apply));
    env_bind(env, make_sym("print"),       make_func(f_print));
    env_bind(env, make_sym("inc"),         make_func(include));
    env_bind(env, make_sym("env"),         env);
    return env;
}

static Expr *env_lookup(Expr *env, Expr *symbol)
{
    assert(is_sym(symbol));
    Expr *bindings = env_bindings(env);
    foreach(bind, bindings) {
        if(sym_eq(symbol, bind_sym(bind))) {
            return bind_val(bind);
        }
    };
    if(is_nil(env_parent(env))) return nil;
    return env_lookup(env_parent(env), symbol);
}

static void env_bind(Expr *env, Expr *symbol, Expr *value)
{
    assert(is_sym(symbol));
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
        list_pushb(env, bind);
    }
}

static void bind_pars(Expr *env, Expr *pars, Expr *args)
{
    if(is_nil(pars)) {
        assert(is_nil(args));
        return;
    }
    if(is_sym(pars)) {
        env_bind(env, pars, args);
        return;
    }
    if(is_pair(pars)) {
        assert(is_pair(args));
        bind_pars(env, car(pars), car(args));
        bind_pars(env, cdr(pars), cdr(args));
    }
}

static Expr *apply(Expr *op, Expr *args)
{
    if(is_func(op)) {
        return func(op)(args);
    }
    else {
        Expr *env  = env_create(closure_env(op));
        Expr *pars = closure_params(op);
        Expr *body = closure_body(op);
        bind_pars(env, pars, args);
        Expr *result = nil;
        Expr *expr_list = body;
        foreach(expr, expr_list) {
            result = eval(env, expr);
        }
        return result;
    }
}

static Expr *evlist(Expr *env, Expr *arg_list)
{
    Expr *evargs = make_nil();
    Expr *args = arg_list;
    foreach(arg, args) {
        Expr *evarg = eval(env, arg);
        list_pushb(evargs, evarg);
    }
    return evargs;
}

static Expr *eval(Expr *env, Expr *expr)
{
    // Literals are evaluated to themselves
    if(atom(expr)) {
        return expr;
    }

    // Symbols are evaluated to whatever they
    // are associated to in the environment.
    if(is_sym(expr)) {
        Expr *def = env_lookup(env, expr);
        assert(def != nil);
        return def;
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
            assert(listn(args) >= 2);
            Expr *pat = car(args);
            Expr *exprs = cdr(args);
            Expr *name;
            Expr *value;
            if(is_sym(pat)) {
                assert(listn(args) == 2);
                name = pat;
                value = eval(env, car(exprs));
            }
            else if(is_pair(pat)) {
                Expr *params = cdr(pat);
                Expr *body = exprs;
                name = car(pat);
                value = make_closure(env, params, body);
            }
            env_bind(env, name, value);
            return value;
        }
        else if(sym_is(op, "\\")) {
            assert(listn(args) >= 2);
            Expr *params = car(args);
            Expr *body = cdr(args);
            return make_closure(env, params, body);
        }
        else if(sym_is(op, "list")) {
            Expr *list = make_nil();
            foreach(arg, args) {
                list_pushb(list, eval(env, arg));
            }
            return list;
        }
        else if(sym_is(op, "cond")) {
            Expr *cases = args;
            foreach(ncase, cases) {
                Expr *cond = eval(env, car(ncase));
                Expr *branch = car(cdr(ncase));
                assert(is_int(cond));
                assert(!is_nil(branch));
                if(val_bool(cond)) {
                    return eval(env, branch);
                }
            }
            assert(is_nil(cases));
            return make_nil();
        }
        else if(sym_is(op, "if")) {
            Expr *cond = eval(env, car(args));
            Expr *branch1 = car(cdr(args));
            Expr *branch0 = car(cdr(cdr(args)));
            if(val_bool(cond)) return eval(env, branch1);
            else               return eval(env, branch0);
        }
        else if(sym_is(op, "and")) {
            Expr *first = eval(env, car(args));
            if(!val_bool(first)) return make_bool(false);
            else return eval(env, car(cdr(args)));
        }
        else if(sym_is(op, "or")) {
            Expr *first = eval(env, car(args));
            if(val_bool(first)) return make_bool(true);
            else return eval(env, car(cdr(args)));
        }
        else if(sym_is(op, "not")) {
            return make_bool(!val_bool(eval(env, car(args))));
        }
        else if(sym_is(op, "do")) {
            Expr *last = make_nil();
            Expr *expr_list = args;
            foreach(expr, expr_list) {
                last = eval(env, expr);
            }
            return last;
        }
    }

    Expr *func = eval(env, op);
    Expr *evargs = evlist(env, args);
    Expr *computed = apply(func, evargs);
    assert(computed != nil);
    return computed;
}

// Expression printing

static Expr *expr_print(Expr *expr)
{
    // Special data
    if(is_closure(expr)) {
        printf("<closure ");
        expr_print(closure_params(expr));
        putchar(' ');
        expr_print(closure_body(expr));
        printf(">");
    }
    else switch(kind(expr)) {
        case EXPR_NIL: printf("nil");                  break;
        case EXPR_SYM: printf("%s",   val_sym(expr));  break;
        case EXPR_INT: printf("%lld", val_int(expr));  break;
        case EXPR_FLT: printf("%f",   val_flt(expr));  break;
        case EXPR_STR: printf("%s", val_str(expr)); break;
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
    }
    return expr;
}

static Expr *expr_println(Expr *expr)
{
    expr_print(expr);
    putchar('\n');
    return expr;
}
