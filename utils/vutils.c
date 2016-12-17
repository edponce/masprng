
#include <stdio.h>
#include "vutils.h"

#if defined(SIMD_MODE)

/*
#define SUCCESS 0
#define ERROR -1


int vload(VECTOR_INT *vdat, const int *dat)
{
    if (!vdat || !dat)
        return ERROR;

#if defined(USE_SSE)
    *vdat = _mm_load_si128((__m128i *)dat); 
#elif USE_AVX
    *vdat = _mm256_load_si256((__m256i *)dat); 
#endif

    return SUCCESS; 
}


int vload(VECTOR_INT *vdat, const unsigned int *dat)
{
    if (!vdat || !dat)
        return ERROR;

#if defined(USE_SSE)
    *vdat = _mm_load_si128((__m128i *)dat); 
#elif USE_AVX
    *vdat = _mm256_load_si256((__m256i *)dat); 
#endif

    return SUCCESS; 
}


int vload(VECTOR_INT *vdat, const long int *dat)
{
    if (!vdat || !dat)
        return ERROR;

#if defined(USE_SSE)
    *vdat = _mm_load_si128((__m128i *)dat); 
#elif USE_AVX
    *vdat = _mm256_load_si256((__m256i *)dat); 
#endif

    return SUCCESS; 
}


int vload(VECTOR_INT *vdat, const unsigned long int *dat)
{
    if (!vdat || !dat)
        return ERROR;

#if defined(USE_SSE)
    *vdat = _mm_load_si128((__m128i *)dat); 
#elif USE_AVX
    *vdat = _mm256_load_si256((__m256i *)dat); 
#endif

    return SUCCESS; 
}


int vload(VECTOR_SP *vdat, const float *dat)
{
    if (!vdat || !dat)
        return ERROR;

#if defined(USE_SSE)
    *vdat = _mm_load_ps((float *)dat); 
#elif USE_AVX
    *vdat = _mm256_load_ps((float *)dat); 
#endif

    return SUCCESS;
}


int vload(VECTOR_DP *vdat, const double *dat)
{
    if (!vdat || !dat)
        return ERROR;

#if defined(USE_SSE)
    *vdat = _mm_load_pd((double *)dat); 
#elif USE_AVX
    *vdat = _mm256_load_pd((double *)dat); 
#endif

    return SUCCESS;
}


int vstore(int *dat, const VECTOR_INT vdat)
{
    if (!dat)
        return ERROR;

#if defined(USE_SSE)
    _mm_store_si128((__m128i *)dat, vdat); 
#elif USE_AVX
    _mm256_store_si256((__m256i *)dat, vdat); 
#endif

    return SUCCESS; 
}


int vstore(unsigned int *dat, const VECTOR_INT vdat)
{
    if (!dat)
        return ERROR;

#if defined(USE_SSE)
    _mm_store_si128((__m128i *)dat, vdat); 
#elif USE_AVX
    _mm256_store_si256((__m256i *)dat, vdat); 
#endif

    return SUCCESS; 
}


int vstore(long int *dat, const VECTOR_INT vdat)
{
    if (!dat)
        return ERROR;

#if defined(USE_SSE)
    _mm_store_si128((__m128i *)dat, vdat); 
#elif USE_AVX
    _mm256_store_si256((__m256i *)dat, vdat); 
#endif

    return SUCCESS; 
}


int vstore(unsigned long int *dat, const VECTOR_INT vdat)
{
    if (!dat)
        return ERROR;

#if defined(USE_SSE)
    _mm_store_si128((__m128i *)dat, vdat); 
#elif USE_AVX
    _mm256_store_si256((__m256i *)dat, vdat); 
#endif

    return SUCCESS; 
}


int vstore(float *dat, const VECTOR_SP vdat)
{
    if (!dat)
        return ERROR;

#if defined(USE_SSE)
    _mm_store_ps((float *)dat, vdat); 
#elif USE_AVX
    _mm256_store_ps((float *)dat, vdat); 
#endif

    return SUCCESS;
}


int vstore(double *dat, const VECTOR_DP vdat)
{
    if (!dat)
        return ERROR;

#if defined(USE_SSE)
    _mm_store_pd((double *)dat, vdat); 
#elif USE_AVX
    _mm256_store_pd((double *)dat, vdat); 
#endif

    return SUCCESS;
}
*/


void vprint(const char *str, const VECTOR_INT vdat)
{
    unsigned int i;

    // int
    int itmp[VECTOR_NUM_32BIT] __attribute__ ((aligned(VECTOR_ALIGN)));
    vstore(itmp, vdat);
    printf("%s", str);
    for (i = 0; i < VECTOR_NUM_32BIT; ++i)
      printf("%d\t", itmp[i]);
    printf("\n");

    // unsigned int
    unsigned int utmp[VECTOR_NUM_32BIT] __attribute__ ((aligned(VECTOR_ALIGN)));
    vstore(utmp, vdat);
    printf("%s", str);
    for (i = 0; i < VECTOR_NUM_32BIT; ++i)
      printf("%u\t", utmp[i]);
    printf("\n");

    // unsigned long 
    unsigned long int lutmp[VECTOR_NUM_64BIT] __attribute__ ((aligned(VECTOR_ALIGN)));
    vstore(lutmp, vdat);
    printf("%s", str);
    for (i = 0; i < VECTOR_NUM_64BIT; ++i)
      printf("%lu\t", lutmp[i]);
    printf("\n");
}


void vprint(const char *str, const VECTOR_SP vdat)
{
    unsigned int i;

    float tmp[VECTOR_NUM_32BIT] __attribute__ ((aligned(VECTOR_ALIGN)));
    vstore(tmp, vdat);
    printf("%s", str);
    for (i = 0; i < VECTOR_NUM_32BIT; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}


void vprint(const char *str, const VECTOR_DP vdat)
{
    unsigned int i;

    double tmp[VECTOR_NUM_64BIT] __attribute__ ((aligned(VECTOR_ALIGN)));
    vstore(tmp, vdat);
    printf("%s", str);
    for (i = 0; i < VECTOR_NUM_64BIT; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}


#endif

