#if defined(USE_SYSTIME)
#include <sys/time.h>
#else
#include <ctime>
#endif


#include <cstdlib>
#include "timers.h"


void startTime(long int * const timers)
{
#if defined(USE_SYSTIME)
    struct timeval tv;

    gettimeofday(&tv, NULL);
    timers[0] = tv.tv_sec;
    timers[1] = tv.tv_usec;
#else
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    timers[0] = ts.tv_sec;
    timers[1] = ts.tv_nsec;
#endif
}


double stopTime(long int * const timers)
{
    double rtime;

#if defined(USE_SYSTIME)
    struct timeval tv;

    gettimeofday(&tv, NULL);
    if (tv.tv_usec >= timers[1]) {
        timers[0] = tv.tv_sec - timers[0];
        timers[1] = tv.tv_usec - timers[1];
    }
    else {
        timers[0] = tv.tv_sec - timers[0];
        timers[1] = timers[1] - tv.tv_usec;
    }
    rtime = timers[0] + timers[1] / 1e6;
#else
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    if (ts.tv_nsec >= timers[1]) {
        timers[0] = ts.tv_sec - timers[0];
        timers[1] = ts.tv_nsec - timers[1];
    }
    else {
        timers[0] = ts.tv_sec - timers[0];
        timers[1] = timers[1] - ts.tv_nsec;
    }
    rtime = timers[0] + timers[1] / 1e9;
#endif

    return rtime;
}

