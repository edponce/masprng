#ifndef __VUTILS_H
#define __VUTILS_H


#if (defined(__GNUC__) || defined(__INTEL_COMPILER)) && defined(__x86_64__)
#include <x86intrin.h>
#endif


// SSE mode
#if defined(USE_SSE)
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
#define NSTRMS 4
#define VECTOR_ALIGN 32 // alignment in bytes
#define VECTOR_INT __m256i
#define VECTOR_SP __m256
#define VECTOR_DP __m256d
#define VECTOR_NUM_32BIT VECTOR_ALIGN/4
#define VECTOR_NUM_64BIT VECTOR_ALIGN/8
#define SIMD_MODE

// Scalar mode
#else
#define NSTRMS 1
#define VECTOR_ALIGN 8 // alignment in bytes
#define VECTOR_INT int 
#define VECTOR_SP float 
#define VECTOR_DP double 
#define VECTOR_NUM_32BIT VECTOR_ALIGN/4
#define VECTOR_NUM_64BIT VECTOR_ALIGN/8
#endif


#if defined(SIMD_MODE)
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
#endif


#endif // __VUTILS_H

