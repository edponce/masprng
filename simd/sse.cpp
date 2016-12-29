#include <stdio.h>
#include "sse.h"


#define SIMD_NUM_32BIT (SIMD_WIDTH_BYTES/4) /*!< number of 32-bit values per vector register */
#define SIMD_NUM_64BIT (SIMD_WIDTH_BYTES/8) /*!< number of 64-bit values per vector register */


void simd_print(const char * const str, const SIMD_INT va)
{
    unsigned int i;

    int itmp[SIMD_NUM_32BIT] __attribute__ ((aligned(SIMD_WIDTH_BYTES)));
    simd_store(itmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_32BIT; ++i)
      printf("%d\t", itmp[i]);
    printf("\n");

    unsigned int utmp[SIMD_NUM_32BIT] __attribute__ ((aligned(SIMD_WIDTH_BYTES)));
    simd_store(utmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_32BIT; ++i)
      printf("%u\t", utmp[i]);
    printf("\n");

    unsigned long int lutmp[SIMD_NUM_64BIT] __attribute__ ((aligned(SIMD_WIDTH_BYTES)));
    simd_store(lutmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_64BIT; ++i)
      printf("%lu\t", lutmp[i]);
    printf("\n");
}


void simd_print(const char * const str, const SIMD_FLT va)
{
    unsigned int i;

    float tmp[SIMD_NUM_32BIT] __attribute__ ((aligned(SIMD_WIDTH_BYTES)));
    simd_store(tmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_32BIT; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}


void simd_print(const char * const str, const SIMD_DBL va)
{
    unsigned int i;

    double tmp[SIMD_NUM_64BIT] __attribute__ ((aligned(SIMD_WIDTH_BYTES)));
    simd_store(tmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_64BIT; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}

