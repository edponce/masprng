#include <cstdio>
#include "sse4_1.h"


/*
 *  Compute number of type elements per vector
 */
#define SIMD_NUM_ELEMS(type) ((int)(SIMD_WIDTH_BYTES/sizeof(type)))


void simd_print(const char * const str, const SIMD_INT va)
{
    int itmp[SIMD_NUM_ELEMS(int)] SIMD_ALIGNED;
    simd_store(itmp, va);
    printf("%s", str);
    for (int i = 0; i < SIMD_NUM_ELEMS(int); ++i)
      printf("%d\t", itmp[i]);
    printf("\n");

    unsigned int utmp[SIMD_NUM_ELEMS(unsigned int)] SIMD_ALIGNED; 
    simd_store(utmp, va);
    printf("%s", str);
    for (int i = 0; i < SIMD_NUM_ELEMS(unsigned int); ++i)
      printf("%u\t", utmp[i]);
    printf("\n");

    unsigned long int lutmp[SIMD_NUM_ELEMS(unsigned long int)] SIMD_ALIGNED;
    simd_store(lutmp, va);
    printf("%s", str);
    for (int i = 0; i < SIMD_NUM_ELEMS(unsigned long int); ++i)
      printf("%lu\t", lutmp[i]);
    printf("\n");
}


void simd_print(const char * const str, const SIMD_FLT va)
{
    float tmp[SIMD_NUM_ELEMS(float)] SIMD_ALIGNED;
    simd_store(tmp, va);
    printf("%s", str);
    for (int i = 0; i < SIMD_NUM_ELEMS(float); ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}


void simd_print(const char * const str, const SIMD_DBL va)
{
    double tmp[SIMD_NUM_ELEMS(double)] SIMD_ALIGNED;
    simd_store(tmp, va);
    printf("%s", str);
    for (int i = 0; i < SIMD_NUM_ELEMS(double); ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}

