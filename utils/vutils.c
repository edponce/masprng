#include "vutils.h"


#define SUCCESS 0
#define ERROR -1


int vload(VECTOR_INT *vdat, int *dat)
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


int vload(VECTOR_INT *vdat, unsigned int *dat)
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


int vload(VECTOR_INT *vdat, long int *dat)
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


int vload(VECTOR_INT *vdat, unsigned long int *dat)
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


int vload(VECTOR_SP *vdat, float *dat)
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


int vload(VECTOR_DP *vdat, double *dat)
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


int vstore(int *dat, VECTOR_INT vdat)
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


int vstore(unsigned int *dat, VECTOR_INT vdat)
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


int vstore(long int *dat, VECTOR_INT vdat)
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


int vstore(unsigned long int *dat, VECTOR_INT vdat)
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


int vstore(float *dat, VECTOR_SP vdat)
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


int vstore(double *dat, VECTOR_DP vdat)
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

