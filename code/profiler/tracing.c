
#if !defined(BLISP_TRACING_BUFSIZE)
    #define BLISP_TRACING_BUFSIZE 1*1024*1024
#endif

#if defined(BLISP_TRACING)
    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    #define NOMINMAX
    #include <Windows.h>
    #define SPALL_IMPLEMENTATION
    #include "spall.h"

    static SpallProfile spall_ctx;
    static SpallBuffer  spall_buffer;

    static double utime() {
        static double invfreq;
        if (!invfreq) {
            LARGE_INTEGER frequency;
            QueryPerformanceFrequency(&frequency);
            invfreq = 1000000.0 / frequency.QuadPart;
        }
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        return counter.QuadPart * invfreq;
    }
    #define trace_startsn(s,n) spall_buffer_begin(&spall_ctx, &spall_buffer, s, n - 1, utime());
    #define trace_end() spall_buffer_end(&spall_ctx, &spall_buffer, utime());
    #define trace_starts(s) trace_startsn(s, sizeof(s)-1)
    #define trace_startf() trace_starts(__FUNCTION__)
    
    static void profiler_init(char *filename) {
        spall_ctx = spall_init_file(filename, 1);
        unsigned char *buffer = malloc(BLISP_TRACING_BUFSIZE);
        spall_buffer = (SpallBuffer){
            .length = BLISP_TRACING_BUFSIZE,
            .data = buffer,
        };
        spall_buffer_init(&spall_ctx, &spall_buffer);
    }
    
    static void profiler_exit() {
        spall_buffer_quit(&spall_ctx, &spall_buffer);
        spall_quit(&spall_ctx);
    }
#else
    #define trace_startsn(s,n)
    #define trace_start(s)
    #define trace_startf()
    #define trace_end()
    #define profiler_init(fn)
    #define profiler_exit()
#endif
