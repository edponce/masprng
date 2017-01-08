#ifndef __AVX2_H
#define __AVX2_H


/*
 *  Compiler and architecture specific settings 
 */
#include "arch.h"


/*
 *  Include supporting header files based on compiler and architecture
 *  NOTE: currently only support x86_64, GCC and Intel compilers
 */
#include <x86intrin.h>


/*
 *  AVX2 256-bit wide vector units 
 *  Define constants required for SIMD module to function properly.
 */
#define SIMD_INT __m256i
#define SIMD_FLT __m256
#define SIMD_DBL __m256d
const int SIMD_WIDTH_BYTES = 32; 


/* Interface Legend
 *
 * simd_*_iXX = signed XX-bit integers
 * simd_*_uXX = unsigned XX-bit integers
 * simd_*_fXX = floating-point XX-bit elements 
 * simd_*_XX  = unsigned/signed XX-bit integers
 * simd_*_XX  = (set functions) specifies width to consider for integer types
 * simd_*     = datatype obtained from function overloading and parameters
 */


// Identify which SIMD functions are required/recommended
#define __SIMD_NEED__
#define __SIMD_RECOMMEND__


/**************************
 *  Arithmetic intrinsics
 **************************/
static inline SIMD_INT simd_add_i32(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{ return _mm_add_epi32(va, vb); }

static inline SIMD_INT simd_add_i64(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{ return _mm_add_epi64(va, vb); }

static inline SIMD_FLT simd_add(const SIMD_FLT va, const SIMD_FLT vb) __SIMD_NEED__
{ return _mm_add_ps(va, vb); }

static inline SIMD_DBL simd_add(const SIMD_DBL va, const SIMD_DBL vb) __SIMD_NEED__
{ return _mm_add_pd(va, vb); }

/*!
 *  Multiply low unsigned 32-bit integers from each packed 64-bit elements
 *  and store the unsigned 64-bit results 
 */
static inline SIMD_INT simd_mul_u32(const SIMD_INT va, const SIMD_INT vb) __SIMD_RECOMMEND__
{ return _mm_mul_epu32(va, vb); }

/*!
 *  Multiply low signed 32-bit integers from each packed 64-bit elements
 *  and store the signed 64-bit results 
 */
static inline SIMD_INT simd_mul_i32(const SIMD_INT va, const SIMD_INT vb) __SIMD_RECOMMEND__
{ return _mm_mul_epi32(va, vb); }

/*!
 *  Perform 64-bit integer multiplication using 32-bit integers
 *  since SSE does not supports 64-bit integer multiplication.
 *  x64 * y64 = (xl * yl) + (xl * yh + xh * yl) * 2^32
 *  NOTE: requires at least SSE 4.1 for _mm_mullo_epi32()
 */
static inline SIMD_INT simd_mul_u64(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{
    const SIMD_INT vfac = _mm_set1_epi64x(0xFFFFFFFF00000000UL);
    SIMD_INT vtmp, vh, vl;

    vtmp = _mm_shuffle_epi32(vb, 0xB1); // shuffle multiplier 
    vh = _mm_mullo_epi32(va, vtmp);     // xl * yh, xh * yl
    vtmp = _mm_slli_epi64(vh, 0x20);    // shift << 32 
    vh = _mm_add_epi64(vh, vtmp);       // h = h1 + h2 
    vh = _mm_and_si128(vh, vfac);       // h & 0xFFFFFFFF00000000
    vl = _mm_mul_epu32(va, vb);         // l = xl * yl

    return _mm_add_epi64(vl, vh);       // l + h
}

/*!
 *  Multiply packed 32-bit integers, produce intermediate 64-bit integers, 
 *  and store the low 32-bit results
 *  NOTE: requires at least SSE 4.1 for _mm_mullo_epi32()
 */
static inline SIMD_INT simd_mullo_i32(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{ return _mm_mullo_epi32(va, vb); }

static inline SIMD_FLT simd_mul(const SIMD_FLT va, const SIMD_FLT vb) __SIMD_NEED__
{ return _mm_mul_ps(va, vb); }

static inline SIMD_DBL simd_mul(const SIMD_DBL va, const SIMD_DBL vb) __SIMD_NEED__
{ return _mm_mul_pd(va, vb); }


/********************************
 *  Integral logical intrinsics
 ********************************/
static inline SIMD_INT simd_or(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{ return _mm_or_si128(va, vb); }

static inline SIMD_INT simd_xor(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{ return _mm_xor_si128(va, vb); }

static inline SIMD_INT simd_and(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__ 
{ return _mm_and_si128(va, vb); }


/*****************************
 *  Shift/Shuffle intrinsics 
 *****************************/
/*
 *  Shift left (logical) packed 32/64-bit integers
 */
static inline SIMD_INT simd_sll_32(const SIMD_INT va, const int shft) __SIMD_NEED__
{ return _mm_slli_epi32(va, shft); }

static inline SIMD_INT simd_srl_32(const SIMD_INT va, const int shft) __SIMD_NEED__ 
{ return _mm_srli_epi32(va, shft); }

static inline SIMD_INT simd_sll_64(const SIMD_INT va, const int shft) __SIMD_NEED__
{ return _mm_slli_epi64(va, shft); }

static inline SIMD_INT simd_srl_64(const SIMD_INT va, const int shft) __SIMD_NEED__
{ return _mm_srli_epi64(va, shft); }

/*!
 *  Shuffle 32-bit integers using control value 
 */
static inline SIMD_INT simd_shuffle_i32(const SIMD_INT va, const int ctrl) __SIMD_NEED__
{ return _mm_shuffle_epi32(va, ctrl); }

static inline SIMD_FLT simd_shuffle_f32(const SIMD_FLT va, const SIMD_FLT vb, const unsigned int ctrl) __SIMD_NEED__
{ return _mm_shuffle_ps(va, vb, ctrl); }


/*******************
 *  Set intrinsics 
 *******************/
/*
 *  Set vector to zero.
 *  Use function overloading to invoke the correct datatype used.
 */
static inline void simd_set_zero(SIMD_INT * const va) __SIMD_NEED__
{ *va = _mm_setzero_si128(); }

static inline void simd_set_zero(SIMD_FLT * const va) __SIMD_NEED__
{ *va = _mm_setzero_ps(); }

static inline void simd_set_zero(SIMD_DBL * const va) __SIMD_NEED__
{ *va = _mm_setzero_pd(); }

/*
 *  Set 32-bit integers to either 32/64 slots.
 */
static inline SIMD_INT simd_set(const int sa) __SIMD_NEED__
{ return _mm_set1_epi32(sa); }

static inline SIMD_INT simd_set_64(const int sa) __SIMD_NEED__
{ return _mm_set1_epi64x(sa); }

static inline SIMD_INT simd_set(const unsigned int sa) __SIMD_NEED__
{ return _mm_set1_epi32(sa); }

static inline SIMD_INT simd_set_64(const unsigned int sa) __SIMD_NEED__
{ return _mm_set1_epi64x(sa); }

static inline SIMD_INT simd_set(const long int sa) __SIMD_NEED__
{ return _mm_set1_epi64x(sa); }

static inline SIMD_INT simd_set(const unsigned long int sa) __SIMD_NEED__
{ return _mm_set1_epi64x(sa); }

static inline SIMD_FLT simd_set(const float sa) __SIMD_NEED__
{ return _mm_set1_ps(sa); }

static inline SIMD_DBL simd_set(const double sa) __SIMD_NEED__
{ return _mm_set1_pd(sa); }

/*
static inline SIMD_INT simd_set(const int sa3, const int sa2, const int sa1, const int sa0)
{ return _mm_set_epi32(sa3, sa2, sa1, sa0); }
static inline SIMD_INT simd_set(const int sa1, const int sa0)
{ return _mm_set_epi64x(sa1, sa0); }
static inline SIMD_INT simd_set(const unsigned int sa3, const unsigned int sa2, const unsigned int sa1, const unsigned int sa0)
{ return _mm_set_epi32(sa3, sa2, sa1, sa0); }
static inline SIMD_INT simd_set(const unsigned int sa1, const unsigned int sa0)
{ return _mm_set_epi64x(sa1, sa0); }
static inline SIMD_INT simd_set(const long int sa1, const long int sa0)
{ return _mm_set_epi64x(sa1, sa0); }
static inline SIMD_INT simd_set(const unsigned long int sa1, const unsigned long int sa0)
{ return _mm_set_epi64x(sa1, sa0); }
static inline SIMD_FLT simd_set(const float sa3, const float sa2, const float sa1, const float sa0)
{ return _mm_set_ps(sa3, sa2, sa1, sa0); }
static inline SIMD_DBL simd_set(const double sa1, const double sa0)
{ return _mm_set_pd(sa1, sa0); }
*/

/*!
 *  Set vector given an array.
 *  Only required for non-contiguous 32-bit elements due to in-between padding,
 *  64-bit elements can use load instructions.
 */
static inline SIMD_INT simd_set(const int * const sa, const int n) __SIMD_NEED__
{
    if (n == 2)
        return _mm_set_epi64x(sa[1], sa[0]);
    else if (n == 4)
        return _mm_load_si128((SIMD_INT *)sa);
    else
        return _mm_setzero_si128();
}

static inline SIMD_INT simd_set(const unsigned int * const sa, const int n) __SIMD_NEED__
{
    if (n == 2)
        return _mm_set_epi64x(sa[1], sa[0]);
    else if (n == 4) 
        return _mm_load_si128((SIMD_INT *)sa);
    else
        return _mm_setzero_si128();
}

static inline SIMD_INT simd_set(const long int * const sa, const int n) __SIMD_NEED__
{
    if (n == 2)
        return _mm_set_epi64x(sa[1], sa[0]);
    else
        return _mm_setzero_si128();
}

static inline SIMD_INT simd_set(const unsigned long int * const sa, const int n) __SIMD_NEED__
{
    if (n == 2)
        return _mm_set_epi64x(sa[1], sa[0]);
    else
        return _mm_setzero_si128();
}


/***********************
 *  Convert intrinsics 
 ***********************/
/*!
 *  Convert packed 32-bit integer elements
 *  to packed single-precision floating-point elements.
 */
static inline SIMD_FLT simd_cvt_i32_f32(const SIMD_INT va) __SIMD_NEED__
{ return _mm_cvtepi32_ps(va); }

/*!
 *  Convert packed 32-bit integer elements
 *  to packed double-precision floating-point elements.
 */
static inline SIMD_DBL simd_cvt_i32_f64(const SIMD_INT va) __SIMD_NEED__
{ return _mm_cvtepi32_pd(va); }

/*!
 *  Convert unsigned 64-bit integers to 32-bit floating-point elements.
 *  NOTE: type conversion performed with scalar FPU since SSE does not support 64-bit integer conversions.
 */
static inline SIMD_FLT simd_cvt_u64_f32(const SIMD_INT va) __SIMD_NEED__
{
    unsigned long int sa[2] SET_ALIGNED(SIMD_WIDTH_BYTES);
    float fa[4] SET_ALIGNED(SIMD_WIDTH_BYTES);
    _mm_store_si128((SIMD_INT *)sa, va);
    fa[0] = (float)sa[0];
    fa[1] = 0.0;
    fa[2] = (float)sa[1];
    fa[3] = 0.0;
    return _mm_load_ps(fa);
}

/*!
 *  Convert unsigned 64-bit integers to 64-bit floating-point elements.
 *  NOTE: type conversion performed with scalar FPU since SSE does not support 64-bit integer conversions.
 */
static inline SIMD_DBL simd_cvt_u64_f64(const SIMD_INT va) __SIMD_NEED__
{
    unsigned long int sa[2] SET_ALIGNED(SIMD_WIDTH_BYTES);
    double fa[2] SET_ALIGNED(SIMD_WIDTH_BYTES);
    _mm_store_si128((SIMD_INT *)sa, va);
    fa[0] = (double)sa[0];
    fa[1] = (double)sa[1];
    return _mm_load_pd(fa);
}


/********************
 *  Load intrinsics 
 ********************/
static inline SIMD_INT simd_load(const int * const sa) __SIMD_NEED__
{ return _mm_load_si128((SIMD_INT *)sa); }

static inline SIMD_INT simd_load(const unsigned int * const sa) __SIMD_NEED__
{ return _mm_load_si128((SIMD_INT *)sa); }

static inline SIMD_INT simd_load(const long int * const sa) __SIMD_NEED__
{ return _mm_load_si128((SIMD_INT *)sa); }

static inline SIMD_INT simd_load(const unsigned long int * const sa) __SIMD_NEED__
{ return _mm_load_si128((SIMD_INT *)sa); }

static inline SIMD_FLT simd_load(const float * const sa) __SIMD_NEED__ 
{ return _mm_load_ps(sa); }

static inline SIMD_DBL simd_load(const double * const sa) __SIMD_NEED__
{ return _mm_load_pd(sa); }


/*******************************
 *  Store intrinsics 
 *******************************/
static inline void simd_store(int * const sa, const SIMD_INT va) __SIMD_NEED__
{ _mm_store_si128((SIMD_INT *)sa, va); }

static inline void simd_store(unsigned int * const sa, const SIMD_INT va) __SIMD_NEED__
{ _mm_store_si128((SIMD_INT *)sa, va); }

static inline void simd_store(long int * const sa, const SIMD_INT va) __SIMD_NEED__
{ _mm_store_si128((SIMD_INT *)sa, va); }

static inline void simd_store(unsigned long int * const sa, const SIMD_INT va) __SIMD_NEED__
{ _mm_store_si128((SIMD_INT *)sa, va); }

static inline void simd_store(float * const sa, const SIMD_FLT va) __SIMD_NEED__
{ _mm_store_ps(sa, va); }

static inline void simd_store(double * const sa, const SIMD_DBL va) __SIMD_NEED__
{ _mm_store_pd(sa, va); }


#endif  // __AVX2_H

