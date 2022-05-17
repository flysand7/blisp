
static Expr *builtin_sum(Expr *args)
{
    i64 sum = 0;
    foreach(arg, args) {
        assert(is_num(arg));
        sum += val_num(arg);
    }
    return make_num(sum);
}

static Expr *builtin_prod(Expr *args)
{
    i64 prod = 1;
    foreach(arg, args) {
        assert(is_num(arg));
        prod *= val_num(arg);
    }
    return make_num(prod);
}

static Expr *builtin_sub(Expr *args)
{
    assert(listn(args) == 2);
    Expr *first = car(args);
    Expr *second = car(cdr(args));
    assert(is_num(first));
    assert(is_num(second));
    i64 result = val_num(first) - val_num(second);
    return make_num(result);
}

static Expr *builtin_div(Expr *args)
{
    assert(listn(args) == 2);
    Expr *first = car(args);
    Expr *second = car(cdr(args));
    assert(is_num(first));
    assert(is_num(second));
    i64 result = val_num(first) / val_num(second);
    return make_num(result);
}

static Expr *builtin_rem(Expr *args)
{
    assert(listn(args) == 2);
    Expr *first = car(args);
    Expr *second = car(cdr(args));
    assert(is_num(first));
    assert(is_num(second));
    i64 result = val_num(first) % val_num(second);
    return make_num(result);
}

static Expr *builtin_less(Expr *args)
{
    assert(listn(args) == 2);
    Expr *first = car(args);
    Expr *second = car(cdr(args));
    assert(is_num(first));
    assert(is_num(second));
    i64 result = val_num(first) < val_num(second);
    return make_num(result);
}
