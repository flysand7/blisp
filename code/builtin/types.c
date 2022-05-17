
static Expr *builtin_is_nil(Expr *expr)
{
    i64 res = is_nil(car(expr));
    return make_num(res);
}

static Expr *builtin_is_list(Expr *expr)
{
    i64 res = is_list(car(expr));
    return make_num(res);
}

static Expr *builtin_is_num(Expr *expr)
{
    i64 res = is_num(car(expr));
    return make_num(res);
}

static Expr *builtin_is_sym(Expr *expr)
{
    i64 res = is_sym(car(expr));
    return make_num(res);
}
