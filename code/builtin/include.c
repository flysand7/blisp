
char *read_file(char *filename)
{
    FILE *file = fopen(filename, "rb");
    if(file == nil) return nil;
    fseek(file, 0, SEEK_END);
    i64 fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *text = malloc(fsize + 1);
    fread(text, fsize, 1, file);
    text[fsize] = 0;
    fclose(file);

    return text;
}

static Expr *run_file(Expr *env, char *filename)
{
    Parser p;
    char *text = read_file(filename);
    if(text == nil) {
        fprintf(stderr, "File %s couldn't be read\n", filename);
        exit(1);
    }
    parser_init(&p, text);
    Expr *code = parse_expr(&p);
    return eval(env, code);
}

static Expr *builtin_include(Expr *args)
{
    Expr *env = car(args);
    Expr *result;
    args = cdr(args);
    expr_println(args);
    assert(!is_nil(args));
    foreach(arg, args) {
        Expr *arg = car(args);
        assert(is_sym(arg));
        char *filename = sym_name(arg);
        result = run_file(env, filename);
    }
    return result;
}