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


/**************************
 *  Arithmetic intrinsics
 **************************/
/*
 *  Add packed 32-bit integers
 */
inline SIMD_INT simd_add_32(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_add_epi32(va, vb); }

/*
 *  Add packed 64-bit integers
 */
inline SIMD_INT simd_add_64(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_add_epi64(va, vb); }

/*
 *  Multiply low unsigned 32-bit integers from each packed 64-bit elements
 *  and store the unsigned 64-bit results 
 */
inline SIMD_INT simd_mul_u32(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_mul_epu32(va, vb); }

/*
 *  Multiply packed 32-bit integers,
 *  produce intermediate 64-bit integers, 
 *  and store the low 32-bit results
 */
inline SIMD_INT simd_mullo_32(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_mullo_epi32(va, vb); }

/*
 *  Multiply packed single-precision floating-point elements
 */
inline SIMD_SP simd_mul_ps(SIMD_SP const va, SIMD_SP const vb)
{ return _mm_mul_ps(va, vb); }

/*
 *  Multiply packed double-precision floating-point elements
 */
inline SIMD_DP simd_mul_pd(SIMD_DP const va, SIMD_DP const vb)
{ return _mm_mul_pd(va, vb); }


/********************************
 *  Integral logical intrinsics
 ********************************/
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

/*
 *  Test if full-vector is zero 
 */
inline int simd_test_zero(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_test_all_zeros(va, vb); }


/***********************
 *  Shift/Shuffle intrinsics 
 ***********************/
/*
 *  Shift left (logical) packed 32-bit integers
 */
inline SIMD_INT simd_sll_32(SIMD_INT const va, int const shft)
{ return _mm_slli_epi32(va, shft); }

/*
 *  Shift right (logical) packed 32-bit integers
 */
inline SIMD_INT simd_srl_32(SIMD_INT const va, int const shft)
{ return _mm_srli_epi32(va, shft); }

/*
 *  Shift left (logical) packed 64-bit integers
 */
inline SIMD_INT simd_sll_64(SIMD_INT const va, int const shft)
{ return _mm_slli_epi64(va, shft); }

/*
 *  Shift right (logical) packed 64-bit integers
 */
inline SIMD_INT simd_srl_64(SIMD_INT const va, int const shft)
{ return _mm_srli_epi64(va, shft); }

/*
 *  Shuffle 32-bit integers using control value 
 */
inline SIMD_INT simd_shuffle_32(SIMD_INT const va, int const ctrl)
{ return _mm_shuffle_epi32(va, ctrl); }


/*******************
 *  Set intrinsics 
 *******************/
/*
 *  Set full-vector to zero 
 */
inline SIMD_INT simd_set_zero(void)
{ return _mm_setzero_si128(); }

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


/***********************
 *  Convert intrinsics 
 ***********************/
/*
 *  Convert packed double-precision floating-point elements
 *  to packed single-precision floating-point elements
 */
inline SIMD_SP simd_cvt_d2s(SIMD_DP const va)
{ return _mm_cvtpd_ps(va); }


/********************
 *  Load intrinsics 
 ********************/
inline SIMD_INT simd_load(int const * const sa)
{ return _mm_load_si128((__m128i *)sa); }

inline SIMD_INT simd_load(unsigned int const * const sa)
{ return _mm_load_si128((__m128i *)sa); }

inline SIMD_INT simd_load(long int const * const sa)
{ return _mm_load_si128((__m128i *)sa); }

inline SIMD_INT simd_load(unsigned long int const * const sa)
{ return _mm_load_si128((__m128i *)sa); }

inline SIMD_INT simd_load(unsigned long long int const * const sa)
{ return _mm_load_si128((__m128i *)sa); }

inline SIMD_SP simd_load(float const * const sa)
{ return _mm_load_ps((float *)sa); }

inline SIMD_DP simd_load(double const * const sa)
{ return _mm_load_pd((double *)sa); }


/*******************************
 *  Store intrinsics 
 *******************************/
inline void simd_store(int * const sa, SIMD_INT const va)
{ _mm_store_si128((__m128i *)sa, va); }

inline void simd_store(unsigned int * const sa, SIMD_INT const va)
{ _mm_store_si128((__m128i *)sa, va); }

inline void simd_store(long int * const sa, SIMD_INT const va)
{ _mm_store_si128((__m128i *)sa, va); }

inline void simd_store(unsigned long int * const sa, SIMD_INT const va)
{ _mm_store_si128((__m128i *)sa, va); }

inline void simd_store(unsigned long long int * const sa, SIMD_INT const va)
{ _mm_store_si128((__m128i *)sa, va); }

inline void simd_store(float * const sa, SIMD_SP const va)
{ _mm_store_ps((float *)sa, va); }

inline void simd_store(double * const sa, SIMD_DP const va)
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
    simd_store(itmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_32BIT; ++i)
      printf("%d\t", itmp[i]);
    printf("\n");

    // unsigned int
    unsigned int utmp[SIMD_NUM_32BIT] __attribute__ ((aligned(SIMD_ALIGN)));
    simd_store(utmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_32BIT; ++i)
      printf("%u\t", utmp[i]);
    printf("\n");

    // unsigned long 
    unsigned long int lutmp[SIMD_NUM_64BIT] __attribute__ ((aligned(SIMD_ALIGN)));
    simd_store(lutmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_64BIT; ++i)
      printf("%lu\t", lutmp[i]);
    printf("\n");
}

void vprint(char const * const str, SIMD_SP const va)
{
    unsigned int i;

    float tmp[SIMD_NUM_32BIT] __attribute__ ((aligned(SIMD_ALIGN)));
    simd_store(tmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_32BIT; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}

void vprint(char const * const str, SIMD_DP const va)
{
    unsigned int i;

    double tmp[SIMD_NUM_64BIT] __attribute__ ((aligned(SIMD_ALIGN)));
    simd_store(tmp, va);
    printf("%s", str);
    for (i = 0; i < SIMD_NUM_64BIT; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}


#endif // USE_SSE

#endif // __SSE_H

