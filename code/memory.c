
#define MEM_POOL_SIZE 0x10000

struct MemBuffer typedef MemBuffer;
struct MemBuffer {
    u64   nallocs;
    Alloc *allocs;
    MemBuffer *next;
};

struct MemPool typedef MemPool;
struct MemPool {
    Alloc *free_head;
    Alloc *alloc_head;
    MemBuffer *buffer;
};

MemPool mem_pool;

static void mem_buffer_create(MemPool *pool)
{
    MemBuffer *buffer = malloc(sizeof(MemBuffer));
    buffer->nallocs = MEM_POOL_SIZE;
    buffer->allocs = malloc(buffer->nallocs * sizeof(Alloc));
    Alloc *buf_end = buffer->allocs + buffer->nallocs;
    Alloc *next = pool->free_head;
    for(Alloc *alloc = buf_end; --alloc >= buffer->allocs;) {
        alloc->next = next;
        next = alloc;
    }
    pool->free_head = next;
    buffer->next = pool->buffer;
    pool->buffer = buffer;
}

static Alloc *mem_pool_alloc(MemPool *pool)
{
    // If not enough space we create a new buffer
    while(pool->free_head == nil) {
        mem_buffer_create(pool);
    }
    // Extract result from the head of free-list
    Alloc *alloc = pool->free_head;
    pool->free_head = alloc->next;
    // Make result a head of the alloc-list
    alloc->next = pool->alloc_head;
    pool->alloc_head = alloc;
    // Unmark the alloc
    alloc->mark = false;
    return alloc;
}

static Alloc *mem_pool_free(MemPool *pool, Alloc *prev, Alloc *alloc) {
    Alloc *next_alloc = alloc->next;
    // Stitch the hole betwee prev and next in the alloc list
    if(prev == nil) {
        pool->alloc_head = alloc->next;
    }
    else {
        prev->next = alloc->next;
    }
    // Remove alloc from alloc list
    // Add alloc to the free list as the head
    alloc->next = pool->free_head;
    pool->free_head = alloc;
    return next_alloc;
}

static void mem_init()
{
    mem_pool.free_head = nil;
    mem_pool.alloc_head = nil;
    mem_buffer_create(&mem_pool);
}

void gc_mark(Expr *expr)
{
    Alloc *alloc = (Alloc *)expr;
    if(alloc->mark) return;
    alloc->mark = true;
    if(is_pair(expr)) {
        gc_mark(car(expr));
        gc_mark(cdr(expr));
    }
}

void gc_sweep() {
    trace_startf();
    Alloc *prev = nil;
    Alloc *alloc = mem_pool.alloc_head;
    while(alloc != nil) {
        if(!alloc->mark) {
            alloc = mem_pool_free(&mem_pool, prev, alloc);
        }
        else {
            prev = alloc;
            alloc = alloc->next;
        }
    }
    for(Alloc *alloc = mem_pool.alloc_head; alloc != nil; alloc=alloc->next) {
        alloc->mark = false;
    }
    trace_end();
}
