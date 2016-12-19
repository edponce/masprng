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
#define SIMD_ALIGN (SIMD_WIDTH_BITS/8) /* alignment in bytes */
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


/*****************************
 *  Shift/Shuffle intrinsics 
 *****************************/
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
 *  Set integer full-vector to zero 
 */
inline void simd_set_zero(SIMD_INT * const va)
{ *va = _mm_setzero_si128(); }

/*
 *  Set single-precision floating-point full-vector to zero 
 */
inline void simd_set_zero(SIMD_SP * const va)
{ *va = _mm_setzero_ps(); }

/*
 *  Set double-precision floating-point full-vector to zero 
 */
inline void simd_set_zero(SIMD_DP * const va)
{ *va = _mm_setzero_pd(); }

/*
 *  Broadcast 64-bit integer to all vector elements
 */
inline SIMD_INT simd_set1_64(int const sa)
{ return _mm_set1_epi64x(sa); }

/*
 *  Broadcast 64-bit integer to all vector elements
 */
inline SIMD_INT simd_set1_64(unsigned int const sa)
{ return _mm_set1_epi64x(sa); }

/*
 *  Broadcast 64-bit integer to all vector elements
 */
inline SIMD_INT simd_set1_64(long int const sa)
{ return _mm_set1_epi64x(sa); }

/*
 *  Broadcast 64-bit integer to all vector elements
 */
inline SIMD_INT simd_set1_64(unsigned long int const sa)
{ return _mm_set1_epi64x(sa); }

/*
 *  Broadcast 64-bit integer to all vector elements
 */
inline SIMD_INT simd_set1_64(long long int const sa)
{ return _mm_set1_epi64x(sa); }

/*
 *  Broadcast 64-bit integer to all vector elements
 */
inline SIMD_INT simd_set1_64(unsigned long long int const sa)
{ return _mm_set1_epi64x(sa); }

/*
 *  Set packed 64-bit integers to corresponding vector elements
 */
inline SIMD_INT simd_set_64(int const sa1, int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }

/*
 *  Set packed 64-bit integers to corresponding vector elements
 */
inline SIMD_INT simd_set_64(unsigned int const sa1, unsigned int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }

/*
 *  Set packed 64-bit integers to corresponding vector elements
 */
inline SIMD_INT simd_set_64(long int const sa1, long int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }

/*
 *  Set packed 64-bit integers to corresponding vector elements
 */
inline SIMD_INT simd_set_64(unsigned long int const sa1, unsigned long int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }

/*
 *  Set packed 64-bit integers to corresponding vector elements
 */
inline SIMD_INT simd_set_64(long long int const sa1, long long int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }

/*
 *  Set packed 64-bit integers to corresponding vector elements
 */
inline SIMD_INT simd_set_64(unsigned long long int const sa1, unsigned long long int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }


/***********************
 *  Convert intrinsics 
 ***********************/
/*
 *  Convert packed double-precision floating-point elements
 *  to packed single-precision floating-point elements
 */
inline SIMD_SP simd_cvt_pd2ps(SIMD_DP const va)
{ return _mm_cvtpd_ps(va); }


/********************
 *  Load intrinsics 
 ********************/
inline SIMD_INT simd_load(int const * const sa)
{ return _mm_load_si128((SIMD_INT *)sa); }

inline SIMD_INT simd_load(unsigned int const * const sa)
{ return _mm_load_si128((SIMD_INT *)sa); }

inline SIMD_INT simd_load(long int const * const sa)
{ return _mm_load_si128((SIMD_INT *)sa); }

inline SIMD_INT simd_load(unsigned long int const * const sa)
{ return _mm_load_si128((SIMD_INT *)sa); }

inline SIMD_INT simd_load(long long int const * const sa)
{ return _mm_load_si128((SIMD_INT *)sa); }

inline SIMD_INT simd_load(unsigned long long int const * const sa)
{ return _mm_load_si128((SIMD_INT *)sa); }

inline SIMD_SP simd_load(float const * const sa)
{ return _mm_load_ps((float *)sa); }

inline SIMD_DP simd_load(double const * const sa)
{ return _mm_load_pd((double *)sa); }


/*******************************
 *  Store intrinsics 
 *******************************/
inline void simd_store(int * const sa, SIMD_INT const va)
{ _mm_store_si128((SIMD_INT *)sa, va); }

inline void simd_store(unsigned int * const sa, SIMD_INT const va)
{ _mm_store_si128((SIMD_INT *)sa, va); }

inline void simd_store(long int * const sa, SIMD_INT const va)
{ _mm_store_si128((SIMD_INT *)sa, va); }

inline void simd_store(unsigned long int * const sa, SIMD_INT const va)
{ _mm_store_si128((SIMD_INT *)sa, va); }

inline void simd_store(long long int * const sa, SIMD_INT const va)
{ _mm_store_si128((SIMD_INT *)sa, va); }

inline void simd_store(unsigned long long int * const sa, SIMD_INT const va)
{ _mm_store_si128((SIMD_INT *)sa, va); }

inline void simd_store(float * const sa, SIMD_SP const va)
{ _mm_store_ps((float *)sa, va); }

inline void simd_store(double * const sa, SIMD_DP const va)
{ _mm_store_pd((double *)sa, va); }



//////////////////////////////////////////////////////////

// NOTE: need to refactor this
#define LONG64 long 

#if defined(LONG64) /* long [long] type for seeds */
#define SIMD_NUM_STREAMS (SIMD_WIDTH_BITS/(8 * sizeof(LONG64)))  /* number of max streams supported */
#endif // LONG64


#endif // USE_SSE


#endif // __SSE_H

