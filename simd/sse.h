#ifndef __SSE_H
#define __SSE_H

// NOTE: find a way to not use this
#if defined(USE_SSE)

/*
 *  Include supporting header files
 */
#include <x86intrin.h>

/*
 *  SSE2 128-bit wide vector units 
 *
 *  Define some constants required for module to function properly.
 */
#define SIMD_WIDTH_BITS 128 /* width in bits of SIMD vector units */
#define SIMD_ALIGN (SIMD_WIDTH_BITS / 8) /* alignment in bytes */
#define SIMD_INT __m128i // integer
#define SIMD_SP __m128 // floating-point single-precision
#define SIMD_DP __m128d // floating-point double-precision

/*
 *  Optional support constants
 */
#define SIMD_NUM_32BIT SIMD_ALIGN/4 // number of 32-bit values per vector register
#define SIMD_NUM_64BIT SIMD_ALIGN/8 // number of 64-bit values per vector register

/*
 *  MASPRNG vector flag
 */
#define SIMD_MODE

#if defined(LONG64) /* long [long] type for seeds */
#define SIMD_NUM_STREAMS (SIMD_WIDTH_BITS / sizeof(LONG64))  /* number of max streams supported */

/*
 *  Left shift 64-bit integers
 */
inline void slli(SIMD_INT *simd, const int shift)
{
    *simd = _mm_slli_epi64(*simd, shift);
}



#endif // LONG64

/*
 *  Load store
 */
int vload(SIMD_INT *, const int *);
int vload(SIMD_INT *, const unsigned int *);
int vload(SIMD_INT *, const long int *);
int vload(SIMD_INT *, const unsigned long int *);
int vload(SIMD_SP *, const float *);
int vload(SIMD_DP *, const double *);
int vstore(int *, const SIMD_INT);
int vstore(unsigned int *, const SIMD_INT);
int vstore(long int *, const SIMD_INT);
int vstore(unsigned long int *, const SIMD_INT);
int vstore(float *, const SIMD_SP);
int vstore(double *, const SIMD_DP);
void vprint(const char *, const SIMD_INT);
void vprint(const char *, const SIMD_SP);
void vprint(const char *, const SIMD_DP);

#endif

#endif // __SSE_H

