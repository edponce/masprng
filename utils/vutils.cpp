#include "simd.h"
#if defined(SIMD_MODE)


#include <stdio.h>
#include "vutils.h" 


void simd_print(const char * const str, const SIMD_INT va)
{
    int32_t itmp[SIMD_STREAMS_32] __SIMD_SET_ALIGNED;
    simd_store(itmp, va);
    printf("%s", str);
    for (int32_t i = 0; i < SIMD_STREAMS_32; ++i)
      printf("%d\t", itmp[i]);
    printf("\n");

    uint32_t utmp[SIMD_STREAMS_32] __SIMD_SET_ALIGNED; 
    simd_store(utmp, va);
    printf("%s", str);
    for (int32_t i = 0; i < SIMD_STREAMS_32; ++i)
      printf("%u\t", utmp[i]);
    printf("\n");

    uint64_t lutmp[SIMD_STREAMS_64] __SIMD_SET_ALIGNED;
    simd_store(lutmp, va);
    printf("%s", str);
    for (int32_t i = 0; i < SIMD_STREAMS_64; ++i)
      printf("%lu\t", lutmp[i]);
    printf("\n");
}


void simd_print(const char * const str, const SIMD_FLT va)
{
    float tmp[SIMD_STREAMS_32] __SIMD_SET_ALIGNED;
    simd_store(tmp, va);
    printf("%s", str);
    for (int32_t i = 0; i < SIMD_STREAMS_32; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}


void simd_print(const char * const str, const SIMD_DBL va)
{
    double tmp[SIMD_STREAMS_64] __SIMD_SET_ALIGNED;
    simd_store(tmp, va);
    printf("%s", str);
    for (int32_t i = 0; i < SIMD_STREAMS_64; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}


#endif // SIMD_MODE

