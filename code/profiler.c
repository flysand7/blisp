
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOMINMAX
#include <Windows.h>
#define SPALL_IMPLEMENTATION
#include "spall.h"

static SpallProfile spall_ctx;
static SpallBuffer  spall_buffer;

double utime() {
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
#if defined(DO_TRACE)
    #define trace_start(s,n) SpallTraceBeginLenTidPid( \
        &spall_ctx,                                 \
        &spall_buffer,                              \
        s,                                          \
        n,                                          \
        0,                                          \
        0,                                          \
        utime()                                     \
        )
    #define trace_end() SpallTraceEndTidPid(&spall_ctx, &spall_buffer, 0, 0, utime())
    #define trace_startf() trace_start(__FUNCTION__, sizeof(__FUNCTION__)-1)
#else
    #define trace_start(s,n)
    #define trace_end()
    #define trace_startf()
#endif
