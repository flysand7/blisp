
// Nil

static Expr *f_is_nil(Expr *args)
{
    assert(listn(args) == 1);
    Expr *expr = list_ith(args, 0);
    return make_bool(is_nil(args));
}

// Integers

static Expr *f_is_int(Expr *args)
{
    assert(listn(args) == 1);
    Expr *expr = list_ith(args, 0);
    return make_bool(is_int(expr));
}

static Expr *f_int_neg(Expr *args)
{
    assert(listn(args) == 1);
    Expr *op1 = list_ith(args, 0);
    assert(is_int(op1));
    i64 i1 = val_int(op1);
    return make_int(-i1);
}

static Expr *f_int_bnot(Expr *args)
{
    assert(listn(args) == 1);
    Expr *op1 = list_ith(args, 0);
    assert(is_int(op1));
    i64 i1 = val_int(op1);
    return make_int(~i1);
}

static Expr *f_int_add(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_int(i1 + i2);
}

static Expr *f_int_sub(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_int(i1 - i2);
}

static Expr *f_int_mul(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_int(i1 * i2);
}

static Expr *f_int_div(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_int(i1 / i2);
}

static Expr *f_int_rem(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_int(i1 % i2);
}

static Expr *f_int_les(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_bool(i1 < i2);
}

static Expr *f_int_grt(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_bool(i1 > i2);
}

static Expr *f_int_eq(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_bool(i1 == i2);
}

static Expr *f_int_neq(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_bool(i1 != i2);
}

static Expr *f_int_leq(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_bool(i1 <= i2);
}

static Expr *f_int_geq(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_bool(i1 >= i2);
}

static Expr *f_int_band(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_int(i1 & i2);
}

static Expr *f_int_bor(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_int(i1 | i2);
}

static Expr *f_int_bxor(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op1 = list_ith(args, 0);
    Expr *op2 = list_ith(args, 1);
    assert(is_int(op1) && is_int(op2));
    i64 i1 = val_int(op1);
    i64 i2 = val_int(op2);
    return make_int(i1 ^ i2);
}

static Expr *f_is_sym(Expr *args)
{
    assert(listn(args) == 1);
    Expr *expr = list_ith(args, 0);
    return make_bool(is_sym(expr));
}

static Expr *f_is_pair(Expr *args)
{
    assert(listn(args) == 1);
    Expr *expr = list_ith(args, 0);
    return make_bool(is_pair(expr));
}

static Expr *f_is_func(Expr *args)
{
    assert(listn(args) == 1);
    Expr *expr = list_ith(args, 0);
    return make_bool(is_func(expr));
}

static Expr *f_is_closure(Expr *args)
{
    assert(listn(args) == 1);
    Expr *expr = list_ith(args, 0);
    return make_bool(is_closure(expr));
}

static Expr *f_is_list(Expr *args)
{
    assert(listn(args) == 1);
    Expr *expr = list_ith(args, 0);
    return make_bool(is_list(expr));
}

static Expr *f_cons(Expr *args)
{
    assert(listn(args) == 2);
    Expr *a = list_ith(args, 0);
    Expr *d = list_ith(args, 1);
    return cons(a, d);
}

static Expr *f_car(Expr *args)
{
    assert(listn(args) == 1);
    Expr *expr = list_ith(args, 0);
    assert(is_pair(expr));
    return car(expr);
}

static Expr *f_cdr(Expr *args)
{
    assert(listn(args) == 1);
    Expr *expr = list_ith(args, 0);
    assert(is_pair(expr));
    return cdr(expr);
}

static Expr *f_eval(Expr *args)
{
    assert(listn(args) == 2);
    Expr *env  = list_ith(args, 0);
    Expr *expr = list_ith(args, 1);
    return eval(env, expr);
}

static Expr *f_apply(Expr *args)
{
    assert(listn(args) == 2);
    Expr *op   = list_ith(args, 0);
    Expr *pars = list_ith(args, 1);
    return apply(op, pars);
}

static Expr *f_print(Expr *args)
{
    Expr *last = nil;
    foreach(arg, args) {
        last = expr_print(arg);
        putchar(' ');
    }
    putchar('\n');
    return last;
}

