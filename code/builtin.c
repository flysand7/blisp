
// Nil

static Expr f_is_nil(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr expr = car(args);
    return expr_bool(is_nil(expr));
}

// Integers

static Expr f_is_int(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr expr = car(args);
    return expr_bool(is_int(expr));
}

static Expr f_int_neg(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr op1 = car(args);
    assert(is_int(op1));
    int64_t i1 = val_int(op1);
    return expr_int(-i1);
}

static Expr f_int_bnot(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr op1 = car(args);
    assert(is_int(op1));
    int64_t i1 = val_int(op1);
    return expr_int(~i1);
}

static Expr f_int_add(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_int(i1 + i2);
}

static Expr f_int_sub(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_int(i1 - i2);
}

static Expr f_int_mul(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_int(i1 * i2);
}

static Expr f_int_div(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_int(i1 / i2);
}

static Expr f_int_rem(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_int(i1 % i2);
}

static Expr f_int_les(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_bool(i1 < i2);
}

static Expr f_int_grt(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_bool(i1 > i2);
}

static Expr f_int_eq(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_bool(i1 == i2);
}

static Expr f_int_neq(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_bool(i1 != i2);
}

static Expr f_int_leq(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_bool(i1 <= i2);
}

static Expr f_int_geq(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_bool(i1 >= i2);
}

static Expr f_int_band(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_int(i1 & i2);
}

static Expr f_int_bor(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_int(i1 | i2);
}

static Expr f_int_bxor(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_int(op1) && is_int(op2));
    int64_t i1 = val_int(op1);
    int64_t i2 = val_int(op2);
    return expr_int(i1 ^ i2);
}

// Floats

static Expr f_is_flt(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr expr = car(args);
    return expr_bool(is_flt(expr));
}

static Expr f_flt_from_int(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr op1 = car(args);
    assert(is_int(op1));
    int64_t i1 = val_int(op1);
    return expr_flt((double)i1);
}

static Expr f_flt_floor(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr op1 = car(args);
    assert(is_flt(op1));
    double f1 = val_flt(op1);
    return expr_int(floor(f1));
}

static Expr f_flt_ceil(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr op1 = car(args);
    assert(is_flt(op1));
    double f1 = val_flt(op1);
    return expr_int(ceil(f1));
}

static Expr f_flt_round(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr op1 = car(args);
    assert(is_flt(op1));
    double f1 = val_flt(op1);
    return expr_int(round(f1));
}

static Expr f_flt_trunc(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr op1 = car(args);
    assert(is_flt(op1));
    double f1 = val_flt(op1);
    return expr_int(trunc(f1));
}

static Expr f_flt_neg(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr op1 = car(args);
    assert(is_flt(op1));
    double f1 = val_flt(op1);
    return expr_flt(-f1);
}

static Expr f_flt_is_inf(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr op1 = car(args);
    assert(is_flt(op1));
    double f1 = val_flt(op1);
    return expr_bool(isinf(f1));
}

static Expr f_flt_is_nan(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr op1 = car(args);
    assert(is_flt(op1));
    double f1 = val_flt(op1);
    return expr_bool(isnan(f1));
}

static Expr f_flt_is_normal(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr op1 = car(args);
    assert(is_flt(op1));
    double f1 = val_flt(op1);
    return expr_bool(isnormal(f1));
}

static Expr f_flt_add(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_flt(op1) && is_flt(op2));
    double f1 = val_flt(op1);
    double f2 = val_flt(op2);
    return expr_flt(f1 + f2);
}

static Expr f_flt_sub(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_flt(op1) && is_flt(op2));
    double f1 = val_flt(op1);
    double f2 = val_flt(op2);
    return expr_flt(f1 - f2);
}

static Expr f_flt_mul(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_flt(op1) && is_flt(op2));
    double f1 = val_flt(op1);
    double f2 = val_flt(op2);
    return expr_flt(f1 * f2);
}

static Expr f_flt_div(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_flt(op1) && is_flt(op2));
    double f1 = val_flt(op1);
    double f2 = val_flt(op2);
    return expr_flt(f1 / f2);
}

static Expr f_flt_les(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_flt(op1) && is_flt(op2));
    double f1 = val_flt(op1);
    double f2 = val_flt(op2);
    return expr_bool(f1 < f2);
}

static Expr f_flt_grt(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_flt(op1) && is_flt(op2));
    double f1 = val_flt(op1);
    double f2 = val_flt(op2);
    return expr_bool(f1 > f2);
}

static Expr f_flt_eq(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_flt(op1) && is_flt(op2));
    double f1 = val_flt(op1);
    double f2 = val_flt(op2);
    return expr_bool(f1 == f2);
}

static Expr f_flt_neq(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_flt(op1) && is_flt(op2));
    double f1 = val_flt(op1);
    double f2 = val_flt(op2);
    return expr_bool(f1 != f2);
}

static Expr f_flt_leq(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_flt(op1) && is_flt(op2));
    double f1 = val_flt(op1);
    double f2 = val_flt(op2);
    return expr_bool(f1 <= f2);
}

static Expr f_flt_geq(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr op1 = car(args);
    Expr op2 = car(cdr(args));
    assert(is_flt(op1) && is_flt(op2));
    double f1 = val_flt(op1);
    double f2 = val_flt(op2);
    return expr_bool(f1 >= f2);
}

// Symbols

static Expr f_is_sym(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr expr = car(args);
    return expr_bool(is_sym(expr));
}

static Expr f_is_sym_eq(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr expr1 = car(args);
    Expr expr2 = car(cdr(args));
    assert(is_sym(expr1));
    assert(is_sym(expr2));
    return expr_bool(sym_eq(expr1, expr2));
}

// Pairs

static Expr f_is_pair(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr expr = car(args);
    return expr_bool(is_pair(expr));
}

// Functions

static Expr f_is_func(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr expr = car(args);
    return expr_bool(is_func(expr));
}

static Expr f_cons(Expr args)
{
    assert(!is_nil(cdr(args)) && is_nil(cdr(cdr(args))));
    Expr a = car(args);
    Expr d = car(cdr(args));
    return cons(a, d);
}

static Expr f_car(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr expr = car(args);
    assert(is_pair(expr));
    return car(expr);
}

static Expr f_cdr(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr expr = car(args);
    assert(is_pair(expr));
    return cdr(expr);
}

static Expr f_input_char(Expr args)
{
    //assert(listn(args) == 0);
    int char_value = getchar();
    assert(char_value != EOF);
    return expr_int(char_value);
}

static Expr f_print_char(Expr args)
{
    assert(is_nil(cdr(args)));
    Expr expr = car(args);
    assert(is_int(expr));
    putchar(val_int(expr));
    return expr_int(val_int(expr));
}

static Expr f_print(Expr args)
{
    Expr last = expr_nil();
    while(!is_nil(args)) {
        Expr arg = car(args);
        last = expr_print(stdout, arg);
        putchar(' ');
        args = cdr(args);
    }
    putchar('\n');
    fflush(stdout);
    return last;
}

