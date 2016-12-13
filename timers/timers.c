#include <stdio.h>
#include <sys/time.h>
#include "timers.h"


// Start timer
void startTime(long long int *timers)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  timers[0] = tv.tv_sec;
  timers[1] = tv.tv_usec;
}


// Stop timer
double stopTime(long long int *timers)
{
    double rtime;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    if (tv.tv_usec >= timers[1]) {
        timers[0] = tv.tv_sec - timers[0];
        timers[1] = tv.tv_usec - timers[1];
        rtime = (double)timers[0] + timers[1] / 1000000.0;
    }
    else {
        timers[0] = tv.tv_sec - timers[0] - 1.0;
        rtime = (double)timers[0] + 1.0 - (timers[1] - tv.tv_usec) / 1000000.0;
    }
    printf("Real time = %.16lf sec\n", rtime);

    return rtime;
}

