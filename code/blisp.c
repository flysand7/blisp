
struct BinFuncDesc typedef BinFuncDesc;
struct BinFuncDesc {
    Func *func;
    Intern *name;
};

static BinFuncDesc *bin_func_list = NULL;
static int64_t bin_func_count = 0;

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

// Nil

static Expr expr_nil()
{
    Expr result;
    kind(result) = EXPR_NIL;
    return result;
}

// Numbers

static Expr expr_int(int64_t value)
{
    Expr result;
    kind(result) = EXPR_INTEGER;
    val_int(result) = value;
    return result;
}

// Floats

static Expr expr_flt(double value)
{
    Expr result;
    kind(result) = EXPR_FLOAT;
    val_flt(result) = value;
    return result;
}

// Booleans

static Expr expr_bool(bool b)
{
    return expr_int(b);
}

static bool val_bool(Expr expr)
{
    return val_int(expr);
}

// Strings

static Expr expr_str(char *str)
{
    Expr result;
    kind(result) = EXPR_STRING;
    val_str(result) = intern_get(&intern_buf, str);
    return result;
}

// Symbols

static Expr expr_sym(char *name)
{
    Expr expr;
    kind(expr) = EXPR_SYMBOL;
    val_sym(expr) = intern_get(&intern_buf, name);
    return expr;
}

static Expr expr_sym_intern(Intern *str) {
    Expr expr;
    kind(expr) = EXPR_SYMBOL;
    val_sym(expr) = str;
    return expr;
}

static bool sym_eq(Expr sym1, Expr sym2)
{
    assert(is_sym(sym1) && is_sym(sym2));
    return val_sym(sym1) == val_sym(sym2);
}

static bool sym_is(Expr sym, Intern *name)
{
    assert(is_sym(sym));
    return val_sym(sym) == name;
}

// Pairs

static Expr cons(Expr car_val, Expr cdr_val)
{
    Expr result;
    result.kind = EXPR_PAIR;
    result.pair = (Pair *)mem_pool_alloc(&mem_pool);
    car(result) = car_val;
    cdr(result) = cdr_val;
    return result;
}

// Lists

static void list_reverse(Expr *list)
{
    Expr tail = expr_nil();
    while(!is_nil(*list)) {
        Expr p = cdr(*list);
        cdr(*list) = tail;
        tail = *list;
        *list = p;
    }
    *list = tail;
}

static Expr list_ith(Expr list, int i)
{
    for(int k = 0; k != i; ++k) {
        list = cdr(list);
    }
    return car(list);
}

static void list_set(Expr list, int i, Expr value) {
    while(i--) {
        list = cdr(list);
    }
    car(list) = value;
}

// C functions

static Expr make_func(Func *f)
{
    Expr expr;
    kind(expr) = EXPR_FUNCTION;
    func(expr) = f;
    return expr;
}

// Closures

static bool is_closure(Expr expr)
{
    return kind(expr) == EXPR_CLOSURE;
}

static Expr make_closure(Expr env, Expr pars, Expr body)
{
    Expr closure = cons(env, cons(pars, cons(body, expr_nil())));
    kind(closure) = EXPR_CLOSURE;
    return closure;
}

// Macros

static bool is_macro(Expr expr)
{
    return kind(expr) == EXPR_MACRO;
}

static Expr make_macro(Expr env, Expr pars, Expr body)
{
    Expr closure = cons(env, cons(pars, cons(body, expr_nil())));
    kind(closure) = EXPR_MACRO;
    return closure;
}


// Environment

static Expr env_create(Expr parent)
{
    return cons(parent, expr_nil());
}

static void add_bin_func(Expr env, char *name, Func *func) {
    bin_func_list = realloc(bin_func_list, (bin_func_count+1)*sizeof(BinFuncDesc));
    Intern *iname = intern_get(&intern_buf, name);
    bin_func_list[bin_func_count++] = (BinFuncDesc) {
        .name = iname,
        .func = func,
    };
    env_bind(env, expr_sym_intern(iname), make_func(func));
}

static Intern *bin_func_name(Func *func) {
    for(int64_t i = 0; i != bin_func_count; ++i) {
        BinFuncDesc desc = bin_func_list[i];
        if(desc.func == func) {
            return desc.name;
        }
    }
    return NULL;
}

static Expr env_default(Expr parent)
{
    Expr env = env_create(parent);

    env_bind(env, expr_sym("nil"),    expr_nil());
    env_bind(env, expr_sym("inf"),    expr_flt(INFINITY));
    env_bind(env, expr_sym("nan"),    expr_flt(NAN));

    add_bin_func(env, "int?",         f_is_int);
    add_bin_func(env, "flt?",         f_is_flt);
    add_bin_func(env, "nil?",         f_is_nil);
    add_bin_func(env, "sym?",         f_is_sym);
    add_bin_func(env, "pair?",        f_is_pair);
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

static bool env_lookup(Expr env, Expr symbol, Expr *value)
{
    assert(is_sym(symbol));
    while(!is_nil(env)) {
        Expr bindings = env_bindings(env);
        while(!is_nil(bindings)) {
            Expr bind = car(bindings);
            if(sym_eq(symbol, bind_sym(bind))) {
                *value = bind_val(bind);
                return true;
            }
            bindings = cdr(bindings);
        };
        env = env_parent(env);
    }
    return false;
}

static void env_bind(Expr env, Expr symbol, Expr value)
{
    assert(is_sym(symbol));
    bool found = false;
    Expr bindings = env_bindings(env);
    while(!is_nil(bindings)) {
        Expr bind = car(bindings);
        if(sym_eq(symbol, bind_sym(bind))) {
            bind_val(bind) = value;
            found = true;
            break;
        }
        bindings = cdr(bindings);
    };
    if(!found) {
        Expr bind = cons(symbol, value);
        env_bindings(env) = cons(bind, env_bindings(env));
    }
}

static void bind_pars(Expr env, Expr pars, Expr args)
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

static Expr make_frame(Expr parent, Expr env, Expr op, Expr args)
{
    Expr frame =
        cons(parent,
        cons(env,
        cons(op,
        cons(args,
        cons(expr_nil(),
        cons(expr_nil(),
        cons(expr_nil(),
        cons(expr_nil(), expr_nil()))))))));
    return frame;
}

static Expr eval_do_exec(Expr *stack, Expr *env)
{
    Expr body;
    *env = frame_env(*stack);
    body = frame_body(*stack);
    Expr expr = car(body);
    body = cdr(body);
    if(is_nil(body)) {
        *stack = car(*stack);
    }
    else {
        frame_body(*stack) = body;
    }
    return expr;
}

static Expr eval_do_bind(Expr *stack, Expr *env)
{
    Expr body = frame_body(*stack);
    if(!is_nil(body)) {
        return eval_do_exec(stack, env);
    }
    Expr op = frame_ev_op(*stack);
    Expr args = frame_ev_arg(*stack);
    *env = env_create(closure_env(op));
    Expr pars = closure_params(op);
    body = closure_body(op);
    frame_env(*stack) = *env;
    frame_body(*stack) = body;
    bind_pars(*env, pars, args);
    frame_ev_arg(*stack) = expr_nil();
    Expr ret = eval_do_exec(stack, env);
    return ret;
}

static Expr eval_do_apply(Expr *stack, Expr *env, Expr *result)
{
    Expr op = frame_ev_op(*stack);
    Expr args = frame_ev_arg(*stack);
    if(!is_nil(args)) {
        list_reverse(&args);
        frame_ev_arg(*stack) = args;
        if(log_file != NULL) {
            static int i = 0;
            if(i < 10000000) {
                Expr ev_args = frame_ev_arg(*stack);
                Expr ev_op = frame_ev_op(*stack);
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
            *stack = make_frame(*stack, *env, op, expr_nil());
            op = car(args);
            args = car(cdr(args));
            frame_ev_op(*stack) = op;
            frame_ev_arg(*stack) = args;
        }
    }
    if(is_func(op)) {
        *stack = frame_parent(*stack);
        return cons(op, args);
    }
    else if(is_closure(op)) {
        Expr ret = eval_do_bind(stack, env);
        return ret;
    }
    else {
        // TODO: I should implement custom format specifiers for this
        // to work properly
        fatal_error("Trying to apply a non-callable object");
        return expr_nil();
    }
}

static Expr eval_do_return(Expr *stack, Expr *env, Expr *result) {
    *env = frame_env(*stack);
    Expr op = frame_ev_op(*stack);
    Expr body = frame_body(*stack);
    if(!is_nil(body)) {
        Expr ret = eval_do_apply(stack, env, result);
        return ret;
    }
    if(is_nil(op)) {
        op = *result;
        frame_ev_op(*stack) = op;
        if(is_macro(op)) {
            Expr args = frame_arg(*stack);
            *stack = make_frame(*stack, *env, op, expr_nil());
            frame_ev_op(*stack) = op;
            frame_ev_arg(*stack) = args;
            Expr ret = eval_do_bind(stack, env);
            return ret;
        }
    }
    else if(is_sym(op)) {
        if(sym_is(op, intern_def)) {
            Expr pattern = frame_ev_arg(*stack);
            env_bind(*env, pattern, *result);
            *stack = car(*stack);
            return cons(expr_sym("quote"), cons(pattern, expr_nil()));
        }
        else if(sym_is(op, intern_if)) {
            Expr args = frame_arg(*stack);
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
        Expr args = frame_ev_arg(*stack);
        frame_ev_arg(*stack) = cons(*result, args);
    }
    Expr args = frame_arg(*stack);
    if(is_nil(args)) {
        Expr ret = eval_do_apply(stack, env, result);
        return ret;
    }
    frame_arg(*stack) = cdr(args);
    return car(args);
}

static Expr eval(Expr env, Expr expr)
{
    Expr stack = expr_nil();
    Expr file_stack = expr_nil();
    Expr result;
    int gc_counter = 0;
    do {
        trace_starts("Eval cycle");
        if(++gc_counter == 1000000) {
            trace_starts("GC cycle");
            gc_mark(expr);
            gc_mark(env);
            gc_mark(stack);
            gc_mark(file_stack);
            gc_sweep();
            gc_counter = 0;
            trace_end();
        }
        // Literals are evaluated to themselves
        if(kind(expr) != EXPR_PAIR && kind(expr) != EXPR_SYMBOL) {
            result = expr;
        }

        // Symbols are evaluated to whatever they
        // are associated to in the environment.
        else if(is_sym(expr)) {
            Expr definition;
            bool is_defined = env_lookup(env, expr, &definition);
            if(!is_defined) {
                trace_end();
                fatal_error("Symbol '%s' is not defined", val_sym(expr)->data);
            }
            result = definition;
        }

        // (op arg1 arg2 ... argn)
        else {
            Expr op   = car(expr);
            Expr args = cdr(expr);
            assert(!is_nil(op));
            // Special forms, e.g quote and define
            if(is_sym(op)) {
                if(sym_is(op, intern_quote)) {
                    // assert(listn(args) == 1);
                    Expr arg = car(args);
                    result = arg;
                }
                else if(sym_is(op, intern_def)) {
                    // assert(listn(args) >= 2);
                    Expr pat = car(args);
                    Expr exprs = cdr(args);
                    Expr name;
                    Expr value;
                    if(is_sym(pat)) {
                        // assert(listn(args) == 2);
                        name = pat;
                        stack = make_frame(stack, env, op, expr_nil());
                        frame_ev_op(stack) = op;
                        frame_ev_arg(stack) = name;
                        expr = car(exprs);
                        trace_end();
                        continue;
                    }
                    else if(is_pair(pat)) {
                        Expr params = cdr(pat);
                        Expr body = exprs;
                        name = car(pat);
                        value = make_closure(env, params, body);
                        env_bind(env, name, value);
                        result = value;
                    }
                }
                else if(sym_is(op, intern_macro)) {
                    // assert(listn(args) >= 2);
                    Expr pat = car(args);
                    Expr exprs = cdr(args);
                    Expr name;
                    Expr value;
                    if(is_pair(pat)) {
                        Expr params = cdr(pat);
                        Expr body = exprs;
                        name = car(pat);
                        value = make_macro(env, params, body);
                        env_bind(env, name, value);
                        result = name;
                    }
                    else {
                        trace_end();
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
                            trace_end();
                            exit(1);
                        }
                        else if(is_nil(cdr(cdr(args)))) {
                            printf("Error: missing if false clause");
                            trace_end();
                            exit(1);
                        }
                    }
                    trace_end();
                    continue;
                }
                else if(sym_is(op, intern_lambda)) {
                    // assert(listn(args) >= 2);
                    Expr params = car(args);
                    Expr body = cdr(args);
                    result = make_closure(env, params, body);
                }
                else if(sym_is(op, intern_include)) {
                    assert(!is_nil(args));
                    Expr arg_list = args;
                    while(!is_nil(arg_list)) {
                        Expr arg = car(arg_list);
                        assert(is_str(arg));
                        char *filename = val_str(arg)->data;
                        Expr stack_frame = cons(arg, cons(stack, cons(expr, expr_nil())));
                        file_stack = cons(stack_frame, file_stack);
                        if(!run_file(env, filename, &result)) {
                            printf("[error]: file %s isn't found\n", filename);
                            trace_end();
                            exit(1);
                        }
                        file_stack = cdr(file_stack);
                        arg_list = cdr(arg_list);
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

static Expr expr_print(FILE *out, Expr expr)
{
    switch(kind(expr)) {
        case EXPR_NIL: {
            fprintf(out, "nil");
        } break;
        case EXPR_SYMBOL: {
            fprintf(out, "%s", val_sym(expr)->data);
        } break;
        case EXPR_INTEGER: {
            fprintf(out, "%lld", val_int(expr));
        } break;
        case EXPR_FLOAT: {
            fprintf(out, "%f", val_flt(expr));
        } break;
        case EXPR_STRING: {
            fprintf(out, "%s", val_str(expr)->data);
        } break;
        case EXPR_FUNCTION: {
            fprintf(out, "%s", bin_func_name(func(expr))->data);
        } break;
        case EXPR_CLOSURE: {
            fprintf(out, "<closure");
            fputc(' ', out);
            expr_print(out, closure_params(expr));
            // expr_print(out, closure_body(expr));
            fprintf(out, ">");
        } break;
        case EXPR_MACRO: {
            fprintf(out, "<macro");
            fputc(' ', out);
            expr_print(out, closure_params(expr));
            // fputc(' ', out);
            // expr_print(out, closure_body(expr));
            fprintf(out, ">");
        } break;
        case EXPR_PAIR: {
            Expr pair = expr;
            fputc('(', out);
            expr_print(out, car(pair));
            pair = cdr(pair);
            while(!is_nil(pair)) {
                if(kind(pair) == EXPR_PAIR) {
                    fputc(' ', out);
                    expr_print(out, car(pair));
                    pair = cdr(pair);
                }
                else {
                    fprintf(out, " . ");
                    expr_print(out, pair);
                    break;
                }
            }
            fputc(')', out);
        } break;
    }
    return expr;
}

static Expr expr_println(FILE *out, Expr expr)
{
    expr_print(out, expr);
    fputc('\n', out);
    return expr;
}
