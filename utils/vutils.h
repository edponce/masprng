#ifndef __VUTILS_H
#define __VUTILS_H


// SSE mode
#if defined(USE_SSE)
#if defined(__SSE4_2__)
#include <nmmintrin.h>  // SSE4.2
#elif defined(__SSE4_1__)
#include <smmintrin.h>  // SSE4.1
#elif defined(__SSSE3__)
#include <tmmintrin.h>  // SSSE3
#elif defined(__SSE3__)
#include <pmmintrin.h>  // SSE3
#elif defined(__SSE2__)
#include <emmintrin.h>  // SSE2
#elif defined(__SSE__)
#include <xmmintrin.h>  // SSE
#endif

#define NSTRMS 2
#define VECTOR_ALIGN 16 // alignment in bytes
#define VECTOR_INT __m128i
#define VECTOR_SP __m128
#define VECTOR_DP __m128d
#define VECTOR_NUM_32BIT VECTOR_ALIGN/4
#define VECTOR_NUM_64BIT VECTOR_ALIGN/8
#define SIMD_MODE

// AVX mode
#elif USE_AVX
#if defined(__AVX512_F__)
#include <immintrin.h>  // AVX, AVX2, AVX-512 (includes SSE4.1)
#elif defined(__AVX2__) || defined(__AVX__)
#include <immintrin.h>  // AVX, AVX2, AVX-512 (includes SSE4.1)
#endif

#define NSTRMS 4
#define VECTOR_ALIGN 32 // alignment in bytes
#define VECTOR_INT __m256i
#define VECTOR_SP __m256
#define VECTOR_DP __m256d
#define VECTOR_NUM_32BIT VECTOR_ALIGN/4
#define VECTOR_NUM_64BIT VECTOR_ALIGN/8
#define SIMD_MODE
#endif


int vload(VECTOR_INT *, const int *);
int vload(VECTOR_INT *, const unsigned int *);
int vload(VECTOR_INT *, const long int *);
int vload(VECTOR_INT *, const unsigned long int *);
int vload(VECTOR_SP *, const float *);
int vload(VECTOR_DP *, const double *);
int vstore(int *, const VECTOR_INT);
int vstore(unsigned int *, const VECTOR_INT);
int vstore(long int *, const VECTOR_INT);
int vstore(unsigned long int *, const VECTOR_INT);
int vstore(float *, const VECTOR_SP);
int vstore(double *, const VECTOR_DP);
void vprint(const char *, const VECTOR_INT);
void vprint(const char *, const VECTOR_SP);
void vprint(const char *, const VECTOR_DP);


#endif // __VUTILS_H

