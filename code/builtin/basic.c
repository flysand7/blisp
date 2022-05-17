
static Expr *builtin_car(Expr *args)
{
    Expr *arg = car(args);
    assert(is_pair(arg));
    return car(arg);
}

static Expr *builtin_cdr(Expr *args)
{
    Expr *arg = car(args);
    assert(is_pair(arg));
    return cdr(arg);
}

static Expr *builtin_cons(Expr *args)
{
    assert(is_list(args));
    Expr *first = car(args);
    Expr *second = car(cdr(args));
    return cons(first, second);
}

static Expr *builtin_eq(Expr *args)
{
    assert(listn(args) == 2);
    Expr *a = car(args);
    Expr *b = car(cdr(args));
    i64 result;
    if(kind(a) != kind(b))     result = 0;
    if(is_nil(a) && is_nil(b)) result = a==b;
    if(is_num(a) && is_num(b)) result = val_num(a) == val_num(b);
    if(is_sym(a) && is_sym(b)) result = sym_eq(a, b);
    return make_num(result);
}

static Expr *builtin_eval(Expr *args)
{
    assert(listn(args) == 2);
    Expr *env = car(args);
    Expr *code = car(cdr(args));
    return eval(env, code);
}

static Expr *builtin_print(Expr *args)
{
    Expr *last = nil;
    foreach(arg, args) {
        expr_println(arg);
        last = arg;
    }
    return last;
}

static Expr *builtin_break(Expr *args)
{
    __debugbreak();
    return args;
}
