#ifndef __SSE_H
#define __SSE_H

// NOTE: find a way to not use this
#if defined(USE_SSE)

/*
 *  Include supporting header files based on compiler and architecture
 */
#define GNUC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if (GNUC_VERSION > 40800 || defined(__INTEL_COMPILER)) && defined(__x86_64__)
#include <x86intrin.h>
#else
// NOTE: currently only support x86_64
#endif

/*
 *  SSE2 128-bit wide vector units 
 *
 *  Define some constants required for module to function properly.
 */
#define SIMD_WIDTH_BITS 128 /* width in bits of SIMD vector units */
#define SIMD_ALIGN (SIMD_WIDTH_BITS / 8) /* alignment in bytes */
#define SIMD_INT __m128i /* integer */
#define SIMD_SP __m128 /* floating-point single-precision */
#define SIMD_DP __m128d /* floating-point double-precision */

/*
 *  MASPRNG vector flag
 *  NOTE: find a way to not set this here
 */
#define SIMD_MODE



/********************************
 *  Integral logical intrinsics
 ********************************/
/*
 *  Shift left (logical) packed 32-bit integers
 */
inline SIMD_INT simd_sll_32(SIMD_INT const va, int const shft)
{ return _mm_slli_epi32(va, shft); }

/*
 *  Shift left (logical) packed 64-bit integers
 */
inline SIMD_INT simd_sll_64(SIMD_INT const va, int const shft)
{ return _mm_slli_epi64(va, shft); }

/*
 *  Bitwise OR full-vector integers
 */
inline SIMD_INT simd_or(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_or_si128(va, vb); }

/*
 *  Bitwise XOR full-vector integers
 */
inline SIMD_INT simd_xor(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_xor_si128(va, vb); }

/*
 *  Bitwise AND full-vector integers
 */
inline SIMD_INT simd_and(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_and_si128(va, vb); }


/***********************
 *  Shuffle intrinsics 
 ***********************/
/*
 *  Shuffle 32-bit integers using control value 
 */
inline SIMD_INT simd_shuffle_32(SIMD_INT const va, int const ctrl)
{ return _mm_shuffle_epi32(va, ctrl); }


/*******************
 *  Set intrinsics 
 *******************/
/*
 *  Broadcast 64-bit integer to all vector elements
 */
inline SIMD_INT simd_set1_64(int const sa)
{ return _mm_set1_epi64x(sa); }

/*
 *  Set packed 64-bit integers to corresponding vector elements
 */
inline SIMD_INT simd_set_64(int const sa1, int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }


/********************
 *  Load intrinsics 
 ********************/
inline SIMD_INT vload(int const * const sa)
{ return _mm_load_si128((__m128i *)sa); }

inline SIMD_INT vload(unsigned int const * const sa)
{ return _mm_load_si128((__m128i *)sa); }

inline SIMD_INT vload(long int const * const sa)
{ return _mm_load_si128((__m128i *)sa); }

inline SIMD_INT vload(unsigned long int const * const sa)
{ return _mm_load_si128((__m128i *)sa); }

inline SIMD_SP vload(float const * const sa)
{ return _mm_load_ps((float *)sa); }

inline SIMD_DP vload(double const * const sa)
{ return _mm_load_pd((double *)sa); }


/*******************************
 *  Store intrinsics 
 *******************************/
inline void vstore(int * const sa, SIMD_INT const va)
{ _mm_store_si128((__m128i *)sa, va); }

inline void vstore(unsigned int * const sa, SIMD_INT const va)
{ _mm_store_si128((__m128i *)sa, va); }

inline void vstore(long int * const sa, SIMD_INT const va)
{ _mm_store_si128((__m128i *)sa, va); }

inline void vstore(unsigned long int * const sa, SIMD_INT const va)
{ _mm_store_si128((__m128i *)sa, va); }

inline void vstore(float * const sa, SIMD_SP const va)
{ _mm_store_ps((float *)sa, va); }

inline void vstore(double * const sa, SIMD_DP const va)
{ _mm_store_pd((double *)sa, va); }



//////////////////////////////////////////////////////////

#if defined(LONG64) /* long [long] type for seeds */
#define SIMD_NUM_STREAMS (SIMD_WIDTH_BITS / sizeof(LONG64))  /* number of max streams supported */
#endif // LONG64

/*
 *  Optional support constants
 */
#define SIMD_NUM_32BIT SIMD_ALIGN/4 /* number of 32-bit values per vector register */
#define SIMD_NUM_64BIT SIMD_ALIGN/8 /* number of 64-bit values per vector register */


/**********************************
 *  SIMD general helper functions
 **********************************/
#include <stdio.h>
void vprint(char const * const str, SIMD_INT const va)
{
    unsigned int i;

    // int
    int itmp[SIMD_NUM_32BIT] __attribute__ ((aligned(SIMD_ALIGN)));
    vstore(itmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_32BIT; ++i)
      printf("%d\t", itmp[i]);
    printf("\n");

    // unsigned int
    unsigned int utmp[SIMD_NUM_32BIT] __attribute__ ((aligned(SIMD_ALIGN)));
    vstore(utmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_32BIT; ++i)
      printf("%u\t", utmp[i]);
    printf("\n");

    // unsigned long 
    unsigned long int lutmp[SIMD_NUM_64BIT] __attribute__ ((aligned(SIMD_ALIGN)));
    vstore(lutmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_64BIT; ++i)
      printf("%lu\t", lutmp[i]);
    printf("\n");
}

void vprint(char const * const str, SIMD_SP const va)
{
    unsigned int i;

    float tmp[SIMD_NUM_32BIT] __attribute__ ((aligned(SIMD_ALIGN)));
    vstore(tmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_32BIT; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}

void vprint(char const * const str, SIMD_DP const va)
{
    unsigned int i;

    double tmp[SIMD_NUM_64BIT] __attribute__ ((aligned(SIMD_ALIGN)));
    vstore(tmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_64BIT; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}


#endif // USE_SSE

#endif // __SSE_H

