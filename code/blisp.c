
struct BinFuncDesc typedef BinFuncDesc;
struct BinFuncDesc {
    Func *func;
    Intern *name;
};

static BinFuncDesc *bin_func_list = nil;
static i64 bin_func_count = 0;

static void fatal_error(char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    printf("[eval]: ");
    vprintf(msg, args);
    putchar('\n');
    longjmp(error_return_buf, 1);
    va_end(args);
}

static void expr_to_atom(Expr *expr)
{
    atom(expr) = true;
}

static Expr *new_expr(ExprKind kind, bool atomic)
{
    Alloc *alloc = mem_pool_alloc(&mem_pool);
    Expr *expr = &alloc->expr;
    kind(expr) = kind;
    atom(expr) = atomic;
    return expr;
}

// Garbage collection

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
    val_str(expr) = intern_get(&intern_buf, str);
    return expr;
}

// Symbols

static Expr *make_sym(char *name)
{
    Expr *expr = new_expr(EXPR_SYM, false);
    val_sym(expr) = intern_get(&intern_buf, name);
    return expr;
}

static Expr *make_sym_intern(Intern *str) {
    Expr *expr = new_expr(EXPR_SYM, false);
    val_sym(expr) = str;
    return expr;
}

static bool sym_eq(Expr *sym1, Expr *sym2)
{
    assert(is_sym(sym1) && is_sym(sym2));
    return val_sym(sym1) == val_sym(sym2);
}

static bool sym_is(Expr *sym, Intern *name)
{
    assert(is_sym(sym));
    return val_sym(sym) == name;
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
    while(is_pair(expr)) {
        expr = cdr(expr);
    }
    if(is_nil(expr)) {
        return true;
    }
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
    if(is_pair(expr)) {
        Expr *flag = car(expr);
        if(is_sym(flag) && sym_is(flag, intern_closure)) {
            return true;
        }
    }
    return false;
}

static Expr *make_closure(Expr *env, Expr *pars, Expr *body)
{
    assert(is_pair(body));
    Expr *closure = cons(make_sym_intern(intern_closure), cons(env, cons(pars, cons(body, make_nil()))));
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

static void add_bin_func(Expr *env, char *name, Func *func) {
    bin_func_list = realloc(bin_func_list, (bin_func_count+1)*sizeof(BinFuncDesc));
    Intern *iname = intern_get(&intern_buf, name);
    bin_func_list[bin_func_count++] = (BinFuncDesc) {
        .name = iname,
        .func = func,
    };
    env_bind(env, make_sym_intern(iname), make_func(func));
}

static Intern *bin_func_name(Func *func) {
    for(i64 i = 0; i != bin_func_count; ++i) {
        BinFuncDesc desc = bin_func_list[i];
        if(desc.func == func) {
            return desc.name;
        }
    }
    return nil;
}

static Expr *env_default(Expr *parent)
{
    Expr *env = env_create(parent);

    env_bind(env, make_sym("nil"),    make_nil());
    env_bind(env, make_sym("inf"),    make_flt(INFINITY));
    env_bind(env, make_sym("nan"),    make_flt(NAN));

    add_bin_func(env, "int?",         f_is_int);
    add_bin_func(env, "flt?",         f_is_flt);
    add_bin_func(env, "nil?",         f_is_nil);
    add_bin_func(env, "sym?",         f_is_sym);
    add_bin_func(env, "pair?",        f_is_pair);
    add_bin_func(env, "list?",        f_is_list);
    add_bin_func(env, "func?",        f_is_func);

    // Integers
    add_bin_func(env, "int-bnot",     f_int_bnot);
    add_bin_func(env, "int-neg",      f_int_neg);
    add_bin_func(env, "int-add",      f_int_add);
    add_bin_func(env, "int-sub",      f_int_sub);
    add_bin_func(env, "int-mul",      f_int_mul);
    add_bin_func(env, "int-div",      f_int_div);
    add_bin_func(env, "int-rem",      f_int_rem);
    add_bin_func(env, "int-les?",     f_int_les);
    add_bin_func(env, "int-grt?",     f_int_grt);
    add_bin_func(env, "int-eq?",      f_int_eq);
    add_bin_func(env, "int-neq?",     f_int_neq);
    add_bin_func(env, "int-leq?",     f_int_leq);
    add_bin_func(env, "int-geq?",     f_int_geq);
    add_bin_func(env, "int-band",     f_int_band);
    add_bin_func(env, "int-bor",      f_int_bor);
    add_bin_func(env, "int-bxor",     f_int_bxor);

    // Floats
    add_bin_func(env, "flt-from-int", f_flt_from_int);
    add_bin_func(env, "flt-floor",    f_flt_floor);
    add_bin_func(env, "flt-ceil",     f_flt_ceil);
    add_bin_func(env, "flt-round",    f_flt_round);
    add_bin_func(env, "flt-trunc",    f_flt_trunc);
    add_bin_func(env, "flt-neg",      f_flt_neg);
    add_bin_func(env, "flt-inf?",     f_flt_is_inf);
    add_bin_func(env, "flt-nan?",     f_flt_is_nan);
    add_bin_func(env, "flt-normal?",  f_flt_is_normal);
    add_bin_func(env, "flt-add",      f_flt_add);
    add_bin_func(env, "flt-sub",      f_flt_sub);
    add_bin_func(env, "flt-mul",      f_flt_mul);
    add_bin_func(env, "flt-div",      f_flt_div);
    add_bin_func(env, "flt-les?",     f_flt_les);
    add_bin_func(env, "flt-grt?",     f_flt_grt);
    add_bin_func(env, "flt-eq?",      f_flt_eq);
    add_bin_func(env, "flt-neq?",     f_flt_neq);
    add_bin_func(env, "flt-leq?",     f_flt_leq);
    add_bin_func(env, "flt-geq?",     f_flt_geq);

    // Symbols
    add_bin_func(env, "sym-eq?",      f_is_sym_eq);

    // Pairs
    add_bin_func(env, "car",          f_car);
    add_bin_func(env, "cdr",          f_cdr);
    add_bin_func(env, "cons",         f_cons);

    add_bin_func(env, "print-ch",     f_print_char);
    add_bin_func(env, "input-ch",     f_input_char);
    add_bin_func(env, "print",        f_print);
    return env;
}

static Expr *env_lookup(Expr *env, Expr *symbol)
{
    assert(is_sym(symbol));
    while(!is_nil(env)) {
        Expr *bindings = env_bindings(env);
        foreach(bind, bindings) {
            if(sym_eq(symbol, bind_sym(bind))) {
                trace_end();
                return bind_val(bind);
            }
        };
        env = env_parent(env);
    }
    return nil;
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
        env_bindings(env) = cons(bind, env_bindings(env));
    }
}

static void bind_pars(Expr *env, Expr *pars, Expr *args)
{
bind:
    if(is_nil(pars)) {
        if(!is_nil(args)) {
            fatal_error("Too much arguments to call a function.");
        }
        return;
    }
    if(is_sym(pars)) {
        env_bind(env, pars, args);
        trace_end();
        return;
    }
    if(is_pair(pars)) {
        if(is_nil(args)) {
            fatal_error("Not enough arguments to call a function.");
        }
        bind_pars(env, car(pars), car(args));
        pars = cdr(pars);
        args = cdr(args);
        goto bind;
    }
}

static Expr *make_frame(Expr *parent, Expr *env, Expr *op, Expr *args)
{
    trace_startf();
    Expr *frame =
        cons(parent,
        cons(env,
        cons(op,
        cons(args,
        cons(make_nil(),
        cons(make_nil(),
        cons(make_nil(),
        cons(make_nil(), make_nil()))))))));
    trace_end();
    return frame;
}

static Expr *eval_do_exec(Expr **stack, Expr **env)
{
    trace_startf();
    Expr *body;
    *env = frame_env(*stack);
    body = frame_body(*stack);
    Expr *expr = car(body);
    body = cdr(body);
    if(is_nil(body)) {
        *stack = car(*stack);
    }
    else {
        frame_body(*stack) = body;
    }
    trace_end();
    return expr;
}

static Expr *eval_do_bind(Expr **stack, Expr **env)
{
    trace_startf();
    Expr *body = frame_body(*stack);
    if(!is_nil(body)) {
        return eval_do_exec(stack, env);
    }
    Expr *op = frame_ev_op(*stack);
    Expr *args = frame_ev_arg(*stack);
    *env = env_create(closure_env(op));
    Expr *pars = closure_params(op);
    body = closure_body(op);
    frame_env(*stack) = *env;
    frame_body(*stack) = body;
    bind_pars(*env, pars, args);
    frame_ev_arg(*stack) = make_nil();
    Expr *ret = eval_do_exec(stack, env);
    trace_end();
    return ret;
}

static Expr *eval_do_apply(Expr **stack, Expr **env, Expr **result)
{
    trace_startf();
    Expr *op = frame_ev_op(*stack);
    Expr *args = frame_ev_arg(*stack);
    if(!is_nil(args)) {
        list_reverse(&args);
        frame_ev_arg(*stack) = args;
        if(log_file != nil) {
            static int i = 0;
            if(i < 10000000) {
                Expr *ev_args = frame_ev_arg(*stack);
                Expr *ev_op = frame_ev_op(*stack);
                fprintf(log_file, "UNCACHED: ");
                expr_print(log_file, ev_op);
                fprintf(log_file, ", ARGS: ");
                expr_print(log_file, ev_args);
                fprintf(log_file, "\n");
            }
            ++i;
        }
    }
    if(is_sym(op)) {
        // instead of (apply, op, (args)) we execute (op, args)
        if(sym_is(op, intern_apply)) {
            *stack = car(*stack);
            *stack = make_frame(*stack, *env, op, make_nil());
            op = car(args);
            args = car(cdr(args));
            if(!is_list(args)) {
                fatal_error("Symbol '%s' is not defined", val_str(args)->data);
            }
            frame_ev_op(*stack) = op;
            frame_ev_arg(*stack) = args;
        }
    }
    if(is_func(op)) {
        *stack = frame_parent(*stack);
        trace_end();
        return cons(op, args);
    }
    else if(is_closure(op)) {
        Expr *ret = eval_do_bind(stack, env);
        trace_end();
        return ret;
    }
    else {
        // TODO: I should implement custom format specifiers for this
        // to work properly
        fatal_error("Trying to apply a non-callable object");
        trace_end();
        return make_nil();
    }
}

static Expr *eval_do_return(Expr **stack, Expr **env, Expr **result) {
    trace_startf();
    *env = frame_env(*stack);
    Expr *op = frame_ev_op(*stack);
    Expr *body = frame_body(*stack);
    if(!is_nil(body)) {
        Expr *ret = eval_do_apply(stack, env, result);
        trace_end();
        return ret;
    }
    if(is_nil(op)) {
        op = *result;
        frame_ev_op(*stack) = op;
        if(is_macro(op)) {
            Expr *args = frame_arg(*stack);
            *stack = make_frame(*stack, *env, op, make_nil());
            op->macro = true;
            frame_ev_op(*stack) = op;
            frame_ev_arg(*stack) = args;
            Expr *ret = eval_do_bind(stack, env);
            trace_end();
            return ret;
        }
    }
    else if(is_sym(op)) {
        if(sym_is(op, intern_def)) {
            Expr *pattern = frame_ev_arg(*stack);
            env_bind(*env, pattern, *result);
            *stack = car(*stack);
            trace_end();
            return cons(make_sym("quote"), cons(pattern, make_nil()));
        }
        else if(sym_is(op, intern_if)) {
            Expr *args = frame_arg(*stack);
            *stack = car(*stack);
            if(!is_int(*result)) {
                fatal_error("The if condition must be a bool.");
            }
            trace_end();
            return val_bool(*result)? car(args) : car(cdr(args));
        }
        else {
            goto store_arg;
        }
    }
    else if(is_macro(op)) {
        *stack = car(*stack);
        trace_end();
        return *result;
    }
    else {
store_arg:;
        Expr *args = frame_ev_arg(*stack);
        frame_ev_arg(*stack) = cons(*result, args);
    }
    Expr *args = frame_arg(*stack);
    if(is_nil(args)) {
        Expr *ret = eval_do_apply(stack, env, result);
        trace_end();
        return ret;
    }
    frame_arg(*stack) = cdr(args);
    trace_end();
    return car(args);
}

static Expr *eval(Expr *env, Expr *expr)
{
    Expr *stack = make_nil();
    Expr *file_stack = make_nil();
    Expr *result;
    int gc_counter = 0;
    do {
        trace_startf();
        if(++gc_counter == 1000000) {
            gc_mark(expr);
            gc_mark(env);
            gc_mark(stack);
            gc_mark(file_stack);
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
                fatal_error("Symbol '%s' is not defined", val_sym(expr)->data);
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
                if(sym_is(op, intern_quote)) {
                    assert(listn(args) == 1);
                    Expr *arg = car(args);
                    result = arg;
                }
                else if(sym_is(op, intern_def)) {
                    assert(listn(args) >= 2);
                    Expr *pat = car(args);
                    Expr *exprs = cdr(args);
                    Expr *name;
                    Expr *value;
                    if(is_sym(pat)) {
                        assert(listn(args) == 2);
                        name = pat;
                        stack = make_frame(stack, env, op, make_nil());
                        frame_ev_op(stack) = op;
                        frame_ev_arg(stack) = name;
                        expr = car(exprs);
                        trace_end();
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
                else if(sym_is(op, intern_macro)) {
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
                else if(sym_is(op, intern_if)) {
                    stack = make_frame(stack, env, op, cdr(args));
                    frame_ev_op(stack) = op;
                    expr = car(args);
                    if(!is_nil(cdr(args))) {
                        if(is_nil(car(cdr(args)))) {
                            printf("Error: missing if true clause");
                            exit(1);
                        }
                        else if(is_nil(cdr(cdr(args)))) {
                            printf("Error: missing if false clause");
                            exit(1);
                        }
                    }
                    trace_end();
                    continue;
                }
                else if(sym_is(op, intern_lambda)) {
                    assert(listn(args) >= 2);
                    Expr *params = car(args);
                    Expr *body = cdr(args);
                    result = make_closure(env, params, body);
                }
                else if(sym_is(op, intern_include)) {
                    assert(!is_nil(args));
                    foreach(arg, args) {
                        Expr *arg = car(args);
                        assert(is_str(arg));
                        char *filename = val_str(arg)->data;
                        Expr *stack_frame = list(3, arg, stack, expr);
                        file_stack = cons(stack_frame, file_stack);
                        result = run_file(env, filename);
                        if(result == nil) {
                            printf("[error]: file %s isn't found\n", filename);
                            exit(1);
                        }
                        file_stack = cdr(file_stack);
                    }
                }
                else if(sym_is(op, intern_apply)) {
                    stack = make_frame(stack, env, op, cdr(args));
                    frame_ev_op(stack) = op;
                    expr = car(args);
                    trace_end();
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
                stack = make_frame(stack, env, op, args);
                expr = op;
                trace_end();
                continue;
            }
        }
        if(is_nil(stack)) {
            trace_end();
            break;
        }
        expr = eval_do_return(&stack, &env, &result);
        trace_end();
    } while(true);
    return result;
}

// Expression printing

static Expr *expr_print(FILE *out, Expr *expr)
{
    // Special data
    if(is_macro(expr)) {
        fprintf(out, "<macro");
        fputc(' ', out);
        expr_print(out, closure_params(expr));
        // fputc(' ', out);
        // expr_print(out, closure_body(expr));
        fprintf(out, ">");
    }
    else if(is_closure(expr)) {
        fprintf(out, "<closure");
        fputc(' ', out);
        expr_print(out, closure_params(expr));
        // expr_print(out, closure_body(expr));
        fprintf(out, ">");
    }
    else switch(kind(expr)) {
        case EXPR_NIL: fprintf(out, "nil");                  break;
        case EXPR_SYM: fprintf(out, "%s",   val_sym(expr)->data);  break;
        case EXPR_INT: fprintf(out, "%lld", val_int(expr));  break;
        case EXPR_FLT: fprintf(out, "%f",   val_flt(expr));  break;
        case EXPR_STR: fprintf(out, "%s",   val_str(expr)->data); break;
        case EXPR_FUNC: fprintf(out, "%s", bin_func_name(func(expr))->data); break;
        case EXPR_PAIR: {
            Expr *sexp = expr;
            fputc('(', out);
            expr_print(out, car(sexp));
            sexp = cdr(sexp);
            while(!is_nil(sexp)) {
                if(sexp->kind == EXPR_PAIR) {
                    fputc(' ', out);
                    expr_print(out, car(sexp));
                    sexp = cdr(sexp);
                }
                else {
                    fprintf(out, " . ");
                    expr_print(out, sexp);
                    break;
                }
            }
            fputc(')', out);
        } break;
    }
    return expr;
}

static Expr *expr_println(FILE *out, Expr *expr)
{
    expr_print(out, expr);
    fputc('\n', out);
    return expr;
}
