
Expr *gc_stack;
Alloc *global_allocs = nil;

static void fatal_error(char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    printf("[eval]: ");
    vprintf(msg, args);
    putchar('\n');
    exit(1);
    va_end(args);
}

static void expr_to_atom(Expr *expr)
{
    atom(expr) = true;
}

static Expr *new_expr(ExprKind kind, bool atomic)
{
    Alloc *alloc = calloc(1, sizeof(Alloc));
    alloc->mark = 0;
    alloc->next = global_allocs;
    global_allocs = alloc;
    Expr *expr = &alloc->expr;
    kind(expr) = kind;
    atom(expr) = atomic;
    return expr;
}

// Garbage collection

void gc_stack_push(Expr *expr)
{
    gc_stack = cons(expr, gc_stack);
}

void gc_stack_pop()
{
    gc_stack = cdr(gc_stack);
}

void gc_mark(Expr *expr)
{
    Alloc *alloc = (Alloc *)expr;
    if(alloc->mark) return;
    alloc->mark = true;
    if(is_pair(expr)) {
        gc_mark(car(expr));
        gc_mark(cdr(expr));
    }
}

void gc_sweep() {
    Alloc **link = &global_allocs;

    while(*link != nil) {
        Alloc *alloc = *link;
        if(!alloc->mark) {
            *link = alloc->next;
            free(alloc);
        }
        else {
            link = &alloc->next;
        }
    }

    for(Alloc *alloc = global_allocs; alloc != nil; alloc=alloc->next) {
        alloc->mark = false;
    }
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

static void list_reverse(Expr **list)
{
    Expr *tail = make_nil();
    while(!is_nil(*list)) {
        Expr *p = cdr(*list);
        cdr(*list) = tail;
        tail = *list;
        *list = p;
    }
    *list = tail;
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

static void list_set(Expr *list, i64 i, Expr *value) {
    while(i--) {
        list = cdr(list);
    }
    car(list) = value;
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

// Macros

static bool is_macro(Expr *expr)
{
    return is_closure(expr) && expr->macro;
}

static Expr *make_macro(Expr *env, Expr *pars, Expr *body)
{
    Expr *macro = make_closure(env, pars, body);
    macro->macro = true;
    return macro;
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

    env_bind(env, make_sym("print"),       make_func(f_print));
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
    if(is_nil(env_parent(env))) {
        return nil;
    }
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
        if(!is_nil(args)) {
            fatal_error("Too much arguments to call a function.");
        }
        return;
    }
    if(is_sym(pars)) {
        env_bind(env, pars, args);
        return;
    }
    if(is_pair(pars)) {
        if(is_nil(args)) {
            fatal_error("Not enough arguments to call a function.");
        }
        bind_pars(env, car(pars), car(args));
        bind_pars(env, cdr(pars), cdr(args));
    }
}

static Expr *make_frame(Expr *parent, Expr *env, Expr *tail)
{
    return list(7,
        parent,
        env,
        make_nil(),
        tail,
        make_nil(),
        make_nil(),
        make_nil()
    );
}

static Expr *eval_do_exec(Expr **stack, Expr **env)
{
    Expr *body;
    *env = list_ith(*stack, 1);
    body = list_ith(*stack, 5);
    Expr *expr = car(body);
    body = cdr(body);
    if(is_nil(body)) {
        *stack = car(*stack);
    }
    else {
        list_set(*stack, 5, body);
    }
    return expr;
}

static Expr *eval_do_bind(Expr **stack, Expr **env)
{
    Expr *body = list_ith(*stack, 5);
    if(!is_nil(body)) {
        return eval_do_exec(stack, env);
    }
    Expr *op = list_ith(*stack, 2);
    Expr *args = list_ith(*stack, 4);
    *env = env_create(closure_env(op));
    Expr *pars = closure_params(op);
    body = closure_body(op);
    list_set(*stack, 1, *env);
    list_set(*stack, 5, body);
    bind_pars(*env, pars, args);
    list_set(*stack, 4, make_nil());
    return eval_do_exec(stack, env);
}

static Expr *eval_do_apply(Expr **stack, Expr **env, Expr **result)
{
    Expr *op = list_ith(*stack, 2);
    Expr *args = list_ith(*stack, 4);
    if(!is_nil(args)) {
        list_reverse(&args);
        list_set(*stack, 4, args);
    }
    if(is_sym(op)) {
        // instead of (apply, op, (args)) we execute (op, args)
        if(sym_is(op, "apply")) {
            *stack = car(*stack);
            *stack = make_frame(*stack, *env, make_nil());
            op = car(args);
            args = car(cdr(args));
            if(!is_list(args)) {
                fatal_error("Symbol '%s' is not defined", val_str(args));
            }
            list_set(*stack, 2, op);
            list_set(*stack, 4, args);
        }
    }
    if(is_func(op)) {
        *stack = car(*stack);
        return cons(op, args);
    }
    else if(is_closure(op)) {
        return eval_do_bind(stack, env);
    }
    else {
        // TODO: I should implement custom format specifiers for this
        // to work properly
        fatal_error("Trying to apply a non-callable object");
        return make_nil();
    }
}

static Expr *eval_do_return(Expr **stack, Expr **env, Expr **result) {
    *env = list_ith(*stack, 1);
    Expr *op = list_ith(*stack, 2);
    Expr *body = list_ith(*stack, 5);
    if(!is_nil(body)) {
        return eval_do_apply(stack, env, result);
    }
    if(is_nil(op)) {
        op = *result;
        list_set(*stack, 2, op);
        if(is_macro(op)) {
            Expr *args = list_ith(*stack, 3);
            *stack = make_frame(*stack, *env, make_nil());
            op->macro = true;
            list_set(*stack, 2, op);
            list_set(*stack, 4, args);
            return eval_do_bind(stack, env);
        }
    }
    else if(is_sym(op)) {
        if(sym_is(op, "def")) {
            Expr *name = list_ith(*stack, 4);
            env_bind(*env, name, *result);
            *stack = car(*stack);
            return cons(make_sym("quote"), cons(name, make_nil()));
        }
        else if(sym_is(op, "if")) {
            Expr *args = list_ith(*stack, 3);
            *stack = car(*stack);
            if(!is_int(*result)) {
                fatal_error("The if condition must be a bool.");
            }
            return val_bool(*result)? car(args) : car(cdr(args));
        }
        else {
            goto store_arg;
        }
    }
    else if(is_macro(op)) {
        *stack = car(*stack);
        return *result;
    }
    else {
store_arg:;
        Expr *args = list_ith(*stack, 4);
        list_set(*stack, 4, cons(*result, args));
    }
    Expr *args = list_ith(*stack, 3);
    if(is_nil(args)) {
        return eval_do_apply(stack, env, result);
    }
    list_set(*stack, 3, cdr(args));
    return car(args);
}

static Expr *eval(Expr *env, Expr *expr)
{
    Expr *stack = make_nil();
    Expr *result;
    int gc_counter = 0;
    do {
        if(++gc_counter == 1000) {
            gc_mark(expr);
            gc_mark(env);
            gc_mark(stack);
            gc_sweep();
            gc_counter = 0;
        }
        // Literals are evaluated to themselves
        if(atom(expr)) {
            result = expr;
        }

        // Symbols are evaluated to whatever they
        // are associated to in the environment.
        else if(is_sym(expr)) {
            Expr *def = env_lookup(env, expr);
            if(def == nil) {
                fatal_error("Symbol '%s' is not defined", val_sym(expr));
            }
            result = def;
        }

        // (op arg1 arg2 ... argn)
        else {
            assert(is_list(expr));
            Expr *op   = car(expr);
            Expr *args = cdr(expr);
            assert(!is_nil(op));
            // Special forms, e.g quote and define
            if(is_sym(op)) {
                if(sym_is(op, "quote")) {
                    assert(listn(args) == 1);
                    Expr *arg = car(args);
                    result = arg;
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
                        stack = make_frame(stack, env, make_nil());
                        list_set(stack, 2, op);
                        list_set(stack, 4, name);
                        expr = car(exprs);
                        continue;
                    }
                    else if(is_pair(pat)) {
                        Expr *params = cdr(pat);
                        Expr *body = exprs;
                        name = car(pat);
                        value = make_closure(env, params, body);
                        env_bind(env, name, value);
                        result = value;
                    }
                }
                else if(sym_is(op, "macro")) {
                    assert(listn(args) >= 2);
                    Expr *pat = car(args);
                    Expr *exprs = cdr(args);
                    Expr *name;
                    Expr *value;
                    if(is_pair(pat)) {
                        Expr *params = cdr(pat);
                        Expr *body = exprs;
                        name = car(pat);
                        value = make_macro(env, params, body);
                        env_bind(env, name, value);
                        result = name;
                    }
                    else {
                        fatal_error("Macro can only be a function");
                    }
                }
                else if(sym_is(op, "if")) {
                    stack = make_frame(stack, env, cdr(args));
                    list_set(stack, 2, op);
                    expr = car(args);
                    continue;
                }
                else if(sym_is(op, "\\")) {
                    assert(listn(args) >= 2);
                    Expr *params = car(args);
                    Expr *body = cdr(args);
                    result = make_closure(env, params, body);
                }
                else if(sym_is(op, "inc")) {
                    Expr *result;
                    assert(!is_nil(args));
                    foreach(arg, args) {
                        Expr *arg = car(args);
                        assert(is_str(arg));
                        char *filename = val_str(arg);
                        result = run_file(env, filename);
                    }
                    return result;
                }
                else if(sym_is(op, "apply")) {
                    stack = make_frame(stack, env, cdr(args));
                    list_set(stack, 2, op);
                    expr = car(args);
                    continue;
                }
                else {
                    goto push;
                }
            }
            else if(is_func(op)) {
                result = func(op)(args);
            }
            else {
push:
                stack = make_frame(stack, env, args);
                expr = op;
                continue;
            }
            // Expr *func = eval(env, op);
            // Expr *evargs = evlist(env, args);
            // Expr *computed = apply(func, evargs);
            // assert(computed != nil);
            // return computed;
        }
        if(is_nil(stack)) {
            break;
        }
        expr = eval_do_return(&stack, &env, &result);
    } while(true);
    return result;
}

// Expression printing

static Expr *expr_print(Expr *expr)
{
    // Special data
    if(is_macro(expr)) {
        printf("<macro ");
        expr_print(closure_params(expr));
        putchar(' ');
        expr_print(closure_body(expr));
        printf(">");
    }
    else if(is_closure(expr)) {
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
