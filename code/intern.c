
#define INTERN_BUF_SIZE 0x10000

struct Intern typedef Intern;
struct Intern {
    int64_t len;
    char *data;
};

struct InternBuffer typedef InternBuffer;
struct InternBuffer {
    int64_t max_interns;
    int64_t ninterns;
    Intern *interns;
};

static InternBuffer intern_buf;
Intern *intern_apply;
Intern *intern_lambda;
Intern *intern_closure;
Intern *intern_macro;
Intern *intern_def;
Intern *intern_if;
Intern *intern_quote;
Intern *intern_include;

static Intern *intern_get(InternBuffer *buffer, char *cstr) {
    trace_startf();
    int64_t str_len = strlen(cstr);
    // Search the intern buffer for the string that interests us
    for(int64_t i = 0; i != buffer->ninterns; ++i) {
        Intern *intern = &buffer->interns[i];
        bool found = true;
        if(intern->len != str_len) {
            found = false;
        }
        if(strcmp(intern->data, cstr) == 0) {
            trace_end();
            return intern;
        }
    }
    if(buffer->ninterns + 1 > buffer->max_interns) {
        assert(false);
    }
    Intern *intern = &buffer->interns[buffer->ninterns++];
    intern->len = str_len;
    intern->data = cstr;
    trace_end();
    return intern;
}

static void interns_init() {
    intern_buf.max_interns = INTERN_BUF_SIZE;
    intern_buf.ninterns = 0;
    intern_buf.interns = malloc(intern_buf.max_interns);
    intern_apply   = intern_get(&intern_buf, "apply");
    intern_lambda  = intern_get(&intern_buf, "\\");
    intern_closure = intern_get(&intern_buf, "closure");
    intern_macro   = intern_get(&intern_buf, "macro");
    intern_def     = intern_get(&intern_buf, "def");
    intern_if      = intern_get(&intern_buf, "if");
    intern_quote   = intern_get(&intern_buf, "quote");
    intern_include = intern_get(&intern_buf, "include");
}
