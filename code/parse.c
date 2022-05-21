
typedef enum TokenType {
    TOKEN_EOF,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_SYMBOL,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_QUOTE,
    TOKEN_COMMA,
    TOKEN_DOT,
} TokenType;

struct Token typedef Token;
struct Token {
    TokenType type;
    char *str;
    i64 i;
    f64 flt;
};

struct Parser typedef Parser;
struct Parser {
    char *stream;
    Token last_token;
};

bool is_whitespace_char(i64 c)
{
    return 
           c == ' '
        || c == '\t'
        || c == '\r'
        || c == '\n'
        || c == '\t';
}


bool is_digit_char(i64 c)
{
    return '0' <= c && c <= '9';
}

bool is_literal_char(i64 c)
{
    return !(c == '(' || c == ')' || c == 0 || is_whitespace_char(c));
}

i64 char_digit_value(i64 c)
{
    return c - '0';
}

void lex_advance(Parser *p)
{
    p->stream++;
}

i64 lex_last(Parser *p)
{
    return *p->stream;
}


bool lex_is(Parser *p, i64 i)
{
    return lex_last(p) == i;
}

bool lex_is_whitespace(Parser *p)
{
    return is_whitespace_char(lex_last(p));
}

bool lex_is_digit(Parser *p)
{
    return is_digit_char(lex_last(p));
}

bool lex_is_literal(Parser *p)
{
    return is_literal_char(lex_last(p));
}

bool lex_is_eof(Parser *p)
{
    return lex_last(p) == 0;
}

bool lex_match(Parser *p, i64 i)
{
    if(lex_last(p) == i) {
        lex_advance(p);
        return true;
    }
    return false;
}

void lex_match_whitespace(Parser *p)
{
    for(;;) {
        if(lex_is_whitespace(p)) {
            lex_advance(p);
        }
        else if(lex_is(p, ';')) {
            until(lex_is(p, 0) || lex_is(p, '\n')) {
                lex_advance(p);
            }
        }
        else {
            break;
        }
    }
}

bool lex_match_digit(Parser *p)
{
    if(is_digit_char(lex_last(p))) {
        lex_advance(p);
        return true;
    }
    return false;
}

#define token_type(p)      ((p)->last_token.type)
#define token_int_value(p) ((p)->last_token.i)
#define token_flt_value(p) ((p)->last_token.flt)
#define token_str_value(p) ((p)->last_token.str)

char *lex_read_literal(Parser *p)
{
    char *str = 0;
    i64 str_length = 0;
    while(lex_is_literal(p)) {
        i64 new_length = str_length + 1;
        i64 last_char_index = str_length;
        str = realloc(str, new_length);
        str[last_char_index] = lex_last(p);
        lex_advance(p);
        str_length = new_length;
    }
    str = realloc(str, str_length+1);
    str[str_length] = 0;
    return str;
}

void lex_literal(Parser *p)
{
    char *lit = lex_read_literal(p);
    char *end = lit;

    // Try parsing an integer
    i64 ivalue = strtoll(lit, &end, 10);
    assert(errno != ERANGE);
    if(*end != 0) {
        // Try parsing a float
        f64 fvalue = strtod(lit, &end);
        assert(fvalue != HUGE_VAL);
        if(*end != 0) {
            // Parse it as a literal
            token_type(p) = TOKEN_SYMBOL;
            token_str_value(p) = lit;
            return;
        }
        token_type(p) = TOKEN_FLOAT;
        token_flt_value(p) = fvalue;
        return;
    }
    token_type(p) = TOKEN_INTEGER;
    token_int_value(p) = ivalue;
}

void lex_str(Parser *p)
{
    char *str = nil;
    i64 str_len = 0;
    lex_match(p, '"');
    until(lex_is(p, '"') || lex_is_eof(p)) {
        char c = lex_last(p);
        if(lex_match(p, '\\')) {
            c = lex_last(p);
        }
        lex_advance(p);
        str = realloc(str, str_len+1);
        str[str_len++] = c;
    }
    assert(!lex_is_eof(p));
    lex_advance(p);
    str = realloc(str, str_len);
    str[str_len] = 0;
    token_type(p) = TOKEN_STRING;
    token_str_value(p) = str;
}

void parse_next_token(Parser *p)
{
    p->last_token = (Token){0};

    lex_match_whitespace(p);

    if(lex_match(p, '(')) {
        token_type(p) = TOKEN_LPAREN;
    }
    else if(lex_match(p, ')')) {
        token_type(p) = TOKEN_RPAREN;
    }
    else if(lex_match(p, '\'')) {
        token_type(p) = TOKEN_QUOTE;
    }
    else if(lex_match(p, ',')) {
        token_type(p) = TOKEN_COMMA;
    }
    else if(lex_match(p, '.')) {
        token_type(p) = TOKEN_DOT;
    }
    else if(lex_is_eof(p)) {
        token_type(p) = TOKEN_EOF;
    }
    else if(lex_is(p, '"')) {
        lex_str(p);
    }
    else if(lex_is_literal(p)) {
        lex_literal(p);
    }
}

#define token_is_quote(p)  (token_type(p) == TOKEN_QUOTE)
#define token_is_comma(p)  (token_type(p) == TOKEN_COMMA)
#define token_is_dot(p)    (token_type(p) == TOKEN_DOT)
#define token_is_lparen(p) (token_type(p) == TOKEN_LPAREN)
#define token_is_rparen(p) (token_type(p) == TOKEN_RPAREN)
#define token_is_int(p)    (token_type(p) == TOKEN_INTEGER)
#define token_is_flt(p)    (token_type(p) == TOKEN_FLOAT)
#define token_is_str(p)    (token_type(p) == TOKEN_STRING)
#define token_is_sym(p)    (token_type(p) == TOKEN_SYMBOL)

bool token_match(Parser *p, TokenType t)
{
    if(token_type(p) == t) {
        parse_next_token(p);
        return true;
    }

    return false;
}

#define token_match_quote(p)  token_match(p, TOKEN_QUOTE)
#define token_match_comma(p)  token_match(p, TOKEN_COMMA)
#define token_match_dot(p)    token_match(p, TOKEN_DOT)
#define token_match_lparen(p) token_match(p, TOKEN_LPAREN)
#define token_match_rparen(p) token_match(p, TOKEN_RPAREN)

void parser_init(Parser *p, char *text)
{
    p->stream = text;
    parse_next_token(p);
}

Expr *parse_expr(Parser *p)
{
    if(token_match_lparen(p)) {
        Expr *list = make_nil();
        until(token_match_rparen(p)) {
            Expr *expr = parse_expr(p);
            if(token_match_dot(p)) {
                Expr *car_val = expr;
                Expr *cdr_val = parse_expr(p);
                expr = cons(car_val, cdr_val);
                list_plugb(list, expr);
                assert(token_match_rparen(p));
                break;
            }
            else {
                list_pushb(list, expr);
            }
        }
        return list;
    }
    else if(token_is_int(p)) {
        Expr *expr = make_int(token_int_value(p));
        parse_next_token(p);
        return expr;
    }
    else if(token_is_flt(p)) {
        Expr *expr = make_flt(token_flt_value(p));
        parse_next_token(p);
        return expr;
    }
    else if(token_is_str(p)) {
        Expr *expr = make_str(token_str_value(p));
        parse_next_token(p);
        return expr;
    }
    else if(token_is_sym(p)) {
        Expr *expr = make_sym(token_str_value(p));
        parse_next_token(p);
        return expr;
    }
    else if(token_match_quote(p)) {
        Expr *arg = parse_expr(p);
        return cons(make_sym("quote"), cons(arg, make_nil()));
    }
    else assert(false);

    return nil;
}

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

static Expr *include(Expr *args)
{
    Expr *env = car(args);
    Expr *result;
    args = cdr(args);
    assert(!is_nil(args));
    foreach(arg, args) {
        Expr *arg = car(args);
        assert(is_sym(arg));
        char *filename = val_sym(arg);
        result = run_file(env, filename);
    }
    return result;
}
