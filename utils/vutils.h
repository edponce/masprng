#ifndef __VUTILS_H
#define __VUTILS_H


// SSE mode
#if defined(USE_SSE)
// Vector extensions settings 
//#include <xmmintrin.h>  // SSE
//#include <emmintrin.h>  // SSE2
//#include <pmmintrin.h>  // SSE3
//#include <tmmintrin.h>  // SSSE3
//#include <smmintrin.h>  // SSE4.1
#include <nmmintrin.h>  // SSE4.2
#define NSTRMS 2
#define VECTOR_ALIGN 16 // alignment in bytes
#define VECTOR_INT __m128i
#define VECTOR_SP __m128
#define VECTOR_DP __m128d

// AVX mode
#elif USE_AVX
// Vector extensions settings 
#include <immintrin.h>  // AVX, AVX2, AVX-512 (includes SSE4.1)
#define NSTRMS 4
#define VECTOR_ALIGN 32 // alignment in bytes
#define VECTOR_INT __m256i
#define VECTOR_SP __m256
#define VECTOR_DP __m256d
#endif

#define VECTOR_NUM_32BIT VECTOR_ALIGN/4
#define VECTOR_NUM_64BIT VECTOR_ALIGN/8

int vload(VECTOR_INT *, int *);
int vload(VECTOR_INT *, unsigned int *);
int vload(VECTOR_INT *, long int *);
int vload(VECTOR_INT *, unsigned long int *);
int vload(VECTOR_SP *, float *);
int vload(VECTOR_DP *, double *);
int vstore(int *, VECTOR_INT);
int vstore(unsigned int *, VECTOR_INT);
int vstore(long int *, VECTOR_INT);
int vstore(unsigned long int *, VECTOR_INT);
int vstore(float *, VECTOR_SP);
int vstore(double *, VECTOR_DP);

#if defined(DEBUG)
#include <stdio.h>
#define aprint(...) printf(__VA_ARGS__)
#define vprint_i(str, val) \
  do { \
    int vb = VECTOR_NUM_32BIT; \
    int vtmpi[vb] __attribute__ ((aligned(VECTOR_ALIGN))); \
    vstore(vtmpi, val); \
    printf(str " = "); \
    for (int vi = 0; vi < vb; ++vi) \
      printf("%d\t", vtmpi[vi]); \
    printf("\n"); \
  } while(0)
#define vprint_u(str, val) \
  do { \
    int vb = VECTOR_NUM_32BIT; \
    unsigned int vtmpi[vb] __attribute__ ((aligned(VECTOR_ALIGN))); \
    vstore(vtmpi, val); \
    printf(str " = "); \
    for (int vi = 0; vi < vb; ++vi) \
      printf("%u\t", vtmpi[vi]); \
    printf("\n"); \
  } while(0)
#define vprint_lu(str, val) \
  do { \
    int vb = VECTOR_NUM_64BIT; \
    unsigned long int vtmpi[vb] __attribute__ ((aligned(VECTOR_ALIGN))); \
    vstore(vtmpi, val); \
    printf(str " = "); \
    for (int vi = 0; vi < vb; ++vi) \
      printf("%lu\t", vtmpi[vi]); \
    printf("\n"); \
  } while(0)
#define vprint_flt(str, val) \
  do { \
    int vb = VECTOR_NUM_32BIT; \
    float vtmpi[vb] __attribute__ ((aligned(VECTOR_ALIGN))); \
    vstore(vtmpi, val); \
    printf(str " = "); \
    for (int vi = 0; vi < vb; ++vi) \
      printf("%f\t", vtmpi[vi]); \
    printf("\n"); \
  } while(0)
#define vprint_dbl(str, val) \
  do { \
    int vb = VECTOR_NUM_64BIT; \
    double vtmpi[vb] __attribute__ ((aligned(VECTOR_ALIGN))); \
    vstore(vtmpi, val); \
    printf(str " = "); \
    for (int vi = 0; vi < vb; ++vi) \
      printf("%g\t", vtmpi[vi]); \
    printf("\n"); \
  } while(0)
#else
#define aprint(...)
#define vprint_i(str, val)
#define vprint_u(str, val)
#define vprint_lu(str, val)
#define vprint_flt(str, val)
#define vprint_dbl(str, val)
#endif


#endif // __VUTILS_H

