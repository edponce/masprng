#ifndef __SSE_H
#define __SSE_H


/*
 *  Include supporting header files based on compiler and architecture
 *  NOTE: currently only support x86_64, GCC and Intel compilers
 */
#include <x86intrin.h>


/*
 *  SSE2 128-bit wide vector units 
 *
 *  Define some constants required for SIMD module to function properly.
 *  NOTE: these defines are mandatory to interface correctly with MASPRNG
 */
#define SIMD_WIDTH_BYTES 16 
#define SIMD_INT __m128i
#define SIMD_FLT __m128
#define SIMD_DBL __m128d


/**************************
 *  Arithmetic intrinsics
 **************************/
inline SIMD_INT simd_add_32(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_add_epi32(va, vb); }
inline SIMD_INT simd_add_64(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_add_epi64(va, vb); }

/*!
 *  Multiply low unsigned 32-bit integers from each packed 64-bit elements
 *  and store the unsigned 64-bit results 
 */
inline SIMD_INT simd_mul_u32(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_mul_epu32(va, vb); }

/*!
 *  Perform 64-bit integer multiplication using 32-bit integers
 *  since SSE does not supports 64-bit integer multiplication.
 *  x64 * y64 = (xl * yl) + (xl * yh + xh * yl) * 2^32
 *  NOTE: requires at least SSE 4.1 for simd_mullo_epi32()
 */
inline SIMD_INT simd_mul_u64(SIMD_INT const va, SIMD_INT const vb)
{
    SIMD_INT vshf, vh, vl;

    vshf = _mm_shuffle_epi32(vb, 0xB1); // shuffle multiplier 
    vh = _mm_mullo_epi32(va, vshf);     // xl * yh, xh * yl
    vshf = _mm_slli_epi64(vh, 0x20);    // shift << 32 
    vh = _mm_add_epi64(vh, vshf);       // h = h1 + h2 
    vshf = _mm_set1_epi64x(0xFFFFFFFF00000000UL);
    vh = _mm_and_si128(vh, vshf);       // h & 0xFFFFFFFF00000000
    vl = _mm_mul_epu32(va, vb);         // l = xl * yl

    return _mm_add_epi64(vl, vh);       // l + h
}

/*!
 *  Multiply packed 32-bit integers, produce intermediate 64-bit integers, 
 *  and store the low 32-bit results
 *  NOTE: requires at least SSE 4.1 for simd_mullo_epi32()
 */
inline SIMD_INT simd_mullo_32(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_mullo_epi32(va, vb); }
inline SIMD_FLT simd_mul_ps(SIMD_FLT const va, SIMD_FLT const vb)
{ return _mm_mul_ps(va, vb); }
inline SIMD_DBL simd_mul_pd(SIMD_DBL const va, SIMD_DBL const vb)
{ return _mm_mul_pd(va, vb); }


/********************************
 *  Integral logical intrinsics
 ********************************/
inline SIMD_INT simd_or(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_or_si128(va, vb); }
inline SIMD_INT simd_xor(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_xor_si128(va, vb); }
inline SIMD_INT simd_and(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_and_si128(va, vb); }
inline int simd_test_zero(SIMD_INT const va, SIMD_INT const vb)
{ return _mm_test_all_zeros(va, vb); }


/*****************************
 *  Shift/Shuffle intrinsics 
 *****************************/
/*
 *  Shift left (logical) packed 32/64-bit integers
 */
inline SIMD_INT simd_sll_32(SIMD_INT const va, int const shft)
{ return _mm_slli_epi32(va, shft); }
inline SIMD_INT simd_srl_32(SIMD_INT const va, int const shft)
{ return _mm_srli_epi32(va, shft); }
inline SIMD_INT simd_sll_64(SIMD_INT const va, int const shft)
{ return _mm_slli_epi64(va, shft); }
inline SIMD_INT simd_srl_64(SIMD_INT const va, int const shft)
{ return _mm_srli_epi64(va, shft); }
/*!
 *  Shuffle 32-bit integers using control value 
 */
inline SIMD_INT simd_shuffle_32(SIMD_INT const va, int const ctrl)
{ return _mm_shuffle_epi32(va, ctrl); }


/*******************
 *  Set intrinsics 
 *******************/
/*
 *  Set vector to zero and set 32-bit integer to either 32/64 slots.
 */
inline void simd_set_zero(SIMD_INT * const va)
{ *va = _mm_setzero_si128(); }
inline void simd_set_zero(SIMD_FLT * const va)
{ *va = _mm_setzero_ps(); }
inline void simd_set_zero(SIMD_DBL * const va)
{ *va = _mm_setzero_pd(); }
inline SIMD_INT simd_set(int const sa)
{ return _mm_set1_epi32(sa); }
inline SIMD_INT simd_set_64(int const sa)
{ return _mm_set1_epi64x(sa); }
inline SIMD_INT simd_set(unsigned int const sa)
{ return _mm_set1_epi32(sa); }
inline SIMD_INT simd_set_64(unsigned int const sa)
{ return _mm_set1_epi64x(sa); }
inline SIMD_INT simd_set(long int const sa)
{ return _mm_set1_epi64x(sa); }
inline SIMD_INT simd_set(unsigned long int const sa)
{ return _mm_set1_epi64x(sa); }
inline SIMD_INT simd_set(long long int const sa)
{ return _mm_set1_epi64x(sa); }
inline SIMD_INT simd_set(unsigned long long int const sa)
{ return _mm_set1_epi64x(sa); }
inline SIMD_FLT simd_set(float const sa)
{ return _mm_set1_ps(sa); }
inline SIMD_DBL simd_set(double const sa)
{ return _mm_set1_pd(sa); }
inline SIMD_INT simd_set(int const sa3, int const sa2, int const sa1, int const sa0)
{ return _mm_set_epi32(sa3, sa2, sa1, sa0); }
inline SIMD_INT simd_set(int const sa1, int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }
inline SIMD_INT simd_set(unsigned int const sa3, unsigned int const sa2, unsigned int const sa1, unsigned int const sa0)
{ return _mm_set_epi32(sa3, sa2, sa1, sa0); }
inline SIMD_INT simd_set(unsigned int const sa1, unsigned int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }
inline SIMD_INT simd_set(long int const sa1, long int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }
inline SIMD_INT simd_set(unsigned long int const sa1, unsigned long int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }
inline SIMD_INT simd_set(long long int const sa1, long long int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }
inline SIMD_INT simd_set(unsigned long long int const sa1, unsigned long long int const sa0)
{ return _mm_set_epi64x(sa1, sa0); }
inline SIMD_FLT simd_set(float const sa3, float const sa2, float const sa1, float const sa0)
{ return _mm_set_ps(sa3, sa2, sa1, sa0); }
inline SIMD_DBL simd_set(double const sa1, double const sa0)
{ return _mm_set_pd(sa1, sa0); }


/***********************
 *  Convert intrinsics 
 ***********************/
/*!
 *  Convert packed double-precision floating-point elements
 *  to packed single-precision floating-point elements
 */
inline SIMD_FLT simd_cvt_pd2ps(SIMD_DBL const va)
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
inline SIMD_FLT simd_load(float const * const sa)
{ return _mm_load_ps((float *)sa); }
inline SIMD_DBL simd_load(double const * const sa)
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
inline void simd_store(float * const sa, SIMD_FLT const va)
{ _mm_store_ps((float *)sa, va); }
inline void simd_store(double * const sa, SIMD_DBL const va)
{ _mm_store_pd((double *)sa, va); }


/**********************************
 *  SIMD general helper functions
 **********************************/
void simd_print(char const * const, SIMD_INT const);
void simd_print(char const * const, SIMD_FLT const);
void simd_print(char const * const, SIMD_DBL const);


#endif  // __SSE_H

