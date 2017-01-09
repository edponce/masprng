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
#include <stdint.h>


/*
 *  AVX2 256-bit wide vector units 
 *  Define constants required for SIMD module to function properly.
 */
#define SIMD_INT __m256i
#define SIMD_FLT __m256
#define SIMD_DBL __m256d
const int32_t SIMD_WIDTH_BYTES = 32; 
const int32_t SIMD_STREAMS_32 = (SIMD_WIDTH_BYTES/4);
const int32_t SIMD_STREAMS_64 = (SIMD_WIDTH_BYTES/8);


/*
 *  Interface Legend
 *
 *  simd_*_iXX = signed XX-bit integers
 *  simd_*_uXX = unsigned XX-bit integers
 *  simd_*_fXX = floating-point XX-bit elements 
 *  simd_*_XX  = unsigned/signed XX-bit integers
 *  simd_*_XX  = (set functions) specifies width to consider for integer types
 *  simd_*     = datatype obtained from function overloading and parameters
 */


// Identify which SIMD functions are required/recommended
#define __SIMD_NEED__
#define __SIMD_RECOMMEND__


/**************************
 *  Arithmetic intrinsics
 **************************/
static inline SIMD_INT simd_add_i32(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{ return _mm256_add_epi32(va, vb); }

static inline SIMD_INT simd_add_i64(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{ return _mm256_add_epi64(va, vb); }

static inline SIMD_FLT simd_add(const SIMD_FLT va, const SIMD_FLT vb) __SIMD_NEED__
{ return _mm256_add_ps(va, vb); }

static inline SIMD_DBL simd_add(const SIMD_DBL va, const SIMD_DBL vb) __SIMD_NEED__
{ return _mm256_add_pd(va, vb); }

/*!
 *  Multiply low unsigned 32-bit integers from each packed 64-bit elements
 *  and store the unsigned 64-bit results 
 */
static inline SIMD_INT simd_mul_u32(const SIMD_INT va, const SIMD_INT vb) __SIMD_RECOMMEND__
{ return _mm256_mul_epu32(va, vb); }

/*!
 *  Multiply low signed 32-bit integers from each packed 64-bit elements
 *  and store the signed 64-bit results 
 */
static inline SIMD_INT simd_mul_i32(const SIMD_INT va, const SIMD_INT vb) __SIMD_RECOMMEND__
{ return _mm256_mul_epi32(va, vb); }

/*!
 *  Perform 64-bit integer multiplication using 32-bit integers
 *  since SSE does not supports 64-bit integer multiplication.
 *  x64 * y64 = (xl * yl) + (xl * yh + xh * yl) * 2^32
 *  NOTE: requires at least SSE 4.1 for _mm_mullo_epi32()
 */
static inline SIMD_INT simd_mul_u64(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{
    const SIMD_INT vfac = _mm256_set1_epi64x(0xFFFFFFFF00000000UL);
    SIMD_INT vtmp, vh, vl;

    vtmp = _mm256_shuffle_epi32(vb, 0xB1); // shuffle multiplier 
    vh = _mm256_mullo_epi32(va, vtmp);     // xl * yh, xh * yl
    vtmp = _mm256_slli_epi64(vh, 0x20);    // shift << 32 
    vh = _mm256_add_epi64(vh, vtmp);       // h = h1 + h2 
    vh = _mm256_and_si256(vh, vfac);       // h & 0xFFFFFFFF00000000
    vl = _mm256_mul_epu32(va, vb);         // l = xl * yl

    return _mm256_add_epi64(vl, vh);       // l + h
}

/*!
 *  Multiply packed 32-bit integers, produce intermediate 64-bit integers, 
 *  and store the low 32-bit results
 *  NOTE: requires at least SSE 4.1 for _mm_mullo_epi32()
 */
static inline SIMD_INT simd_mullo_i32(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{ return _mm256_mullo_epi32(va, vb); }

static inline SIMD_FLT simd_mul(const SIMD_FLT va, const SIMD_FLT vb) __SIMD_NEED__
{ return _mm256_mul_ps(va, vb); }

static inline SIMD_DBL simd_mul(const SIMD_DBL va, const SIMD_DBL vb) __SIMD_NEED__
{ return _mm256_mul_pd(va, vb); }


/********************************
 *  Integral logical intrinsics
 ********************************/
static inline SIMD_INT simd_or(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{ return _mm256_or_si256(va, vb); }

static inline SIMD_INT simd_xor(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__
{ return _mm256_xor_si256(va, vb); }

static inline SIMD_INT simd_and(const SIMD_INT va, const SIMD_INT vb) __SIMD_NEED__ 
{ return _mm256_and_si256(va, vb); }


/*****************************
 *  Shift/Shuffle intrinsics 
 *****************************/
/*
 *  Shift left (logical) packed 32/64-bit integers
 */
static inline SIMD_INT simd_sll_32(const SIMD_INT va, const int32_t shft) __SIMD_NEED__
{ return _mm256_slli_epi32(va, shft); }

static inline SIMD_INT simd_srl_32(const SIMD_INT va, const int32_t shft) __SIMD_NEED__ 
{ return _mm256_srli_epi32(va, shft); }

static inline SIMD_INT simd_sll_64(const SIMD_INT va, const int32_t shft) __SIMD_NEED__
{ return _mm256_slli_epi64(va, shft); }

static inline SIMD_INT simd_srl_64(const SIMD_INT va, const int32_t shft) __SIMD_NEED__
{ return _mm256_srli_epi64(va, shft); }

/*!
 *  Shuffle 32-bit integers using control value 
 */
static inline SIMD_INT simd_shuffle_i32(const SIMD_INT va, const int32_t ctrl) __SIMD_NEED__
{ return _mm256_shuffle_epi32(va, ctrl); }

static inline SIMD_FLT simd_shuffle_f32(const SIMD_FLT va, const SIMD_FLT vb, const uint32_t ctrl) __SIMD_NEED__
{ return _mm256_shuffle_ps(va, vb, ctrl); }


/*******************
 *  Set intrinsics 
 *******************/
/*
 *  Set vector to zero.
 *  Use function overloading to invoke the correct datatype used.
 */
static inline void simd_set_zero(SIMD_INT * const va) __SIMD_NEED__
{ *va = _mm256_setzero_si256(); }

static inline void simd_set_zero(SIMD_FLT * const va) __SIMD_NEED__
{ *va = _mm256_setzero_ps(); }

static inline void simd_set_zero(SIMD_DBL * const va) __SIMD_NEED__
{ *va = _mm256_setzero_pd(); }

/*
 *  Set 32-bit integers to either 32/64 slots.
 */
static inline SIMD_INT simd_set(const int32_t sa) __SIMD_NEED__
{ return _mm256_set1_epi32(sa); }

static inline SIMD_INT simd_set_64(const int32_t sa) __SIMD_NEED__
{ return _mm256_set1_epi64x((int64_t)sa); }

static inline SIMD_INT simd_set(const uint32_t sa) __SIMD_NEED__
{ return _mm256_set1_epi32((int32_t)sa); }

static inline SIMD_INT simd_set_64(const uint32_t sa) __SIMD_NEED__
{ return _mm256_set1_epi64x((int64_t)sa); }

static inline SIMD_INT simd_set(const int64_t sa) __SIMD_NEED__
{ return _mm256_set1_epi64x(sa); }

static inline SIMD_INT simd_set(const uint64_t sa) __SIMD_NEED__
{ return _mm256_set1_epi64x((int64_t)sa); }

static inline SIMD_FLT simd_set(const float sa) __SIMD_NEED__
{ return _mm256_set1_ps(sa); }

static inline SIMD_DBL simd_set(const double sa) __SIMD_NEED__
{ return _mm256_set1_pd(sa); }

/*!
 *  Set vector given an array.
 *  Only required for non-contiguous 32-bit elements due to in-between padding,
 *  64-bit elements can use load instructions.
 */
static inline SIMD_INT simd_set(const int32_t * const sa, const int32_t n) __SIMD_NEED__
{
    if (n == SIMD_STREAMS_64)
        return _mm256_set_epi64x((int64_t)sa[3], (int64_t)sa[2], (int64_t)sa[1], (int64_t)sa[0]);
    else if (n == SIMD_STREAMS_32)
        return _mm256_load_si256((SIMD_INT *)sa);
    else
        return _mm256_setzero_si256();
}

static inline SIMD_INT simd_set(const uint32_t * const sa, const int32_t n) __SIMD_NEED__
{
    if (n == SIMD_STREAMS_64)
        return _mm256_set_epi64x((int64_t)sa[3], (int64_t)sa[2], (int64_t)sa[1], (int64_t)sa[0]);
    else if (n == SIMD_STREAMS_32)
        return _mm256_load_si256((SIMD_INT *)sa);
    else
        return _mm256_setzero_si256();
}

static inline SIMD_INT simd_set(const int64_t * const sa, const int32_t n) __SIMD_NEED__
{
    if (n == SIMD_STREAMS_64)
        return _mm256_set_epi64x(sa[3], sa[2], sa[1], sa[0]);
    else
        return _mm256_setzero_si256();
}

static inline SIMD_INT simd_set(const uint64_t * const sa, const int32_t n) __SIMD_NEED__
{
    if (n == SIMD_STREAMS_64)
        return _mm256_set_epi64x((int64_t)sa[3], (int64_t)sa[2], (int64_t)sa[1], (int64_t)sa[0]);
    else
        return _mm256_setzero_si256();
}


/***********************
 *  Convert intrinsics 
 ***********************/
/*!
 *  Convert packed 32-bit integer elements
 *  to packed single-precision floating-point elements.
 */
static inline SIMD_FLT simd_cvt_i32_f32(const SIMD_INT va) __SIMD_NEED__
{ return _mm256_cvtepi32_ps(va); }

/*!
 *  Convert packed 32-bit integer elements
 *  to packed double-precision floating-point elements.
 */
static inline SIMD_DBL simd_cvt_i32_f64(const SIMD_INT va) __SIMD_NEED__
{
    const SIMD_FLT fa = _mm256_cvtepi32_ps(va);
    const __m128 da = _mm256_castps256_ps128(fa);
    return _mm256_cvtps_pd(da);
}

/*!
 *  Convert unsigned 64-bit integers to 32-bit floating-point elements.
 *  NOTE: type conversion performed with scalar FPU since SSE does not support 64-bit integer conversions.
 */
static inline SIMD_FLT simd_cvt_u64_f32(const SIMD_INT va) __SIMD_NEED__
{
    uint64_t sa[SIMD_STREAMS_64] SET_ALIGNED(SIMD_WIDTH_BYTES);
    uint64_t *sa_ptr = sa;
    float fa[SIMD_STREAMS_32] SET_ALIGNED(SIMD_WIDTH_BYTES);
    float *fa_ptr = fa; 

    _mm256_store_si256((SIMD_INT *)sa, va);
    for (int32_t i = 0; i < SIMD_STREAMS_64; ++i) {
        *(fa_ptr++) = (float)*(sa_ptr++);
        *(fa_ptr++) = 0.0;
    }

    return _mm256_load_ps(fa);
}

/*!
 *  Convert unsigned 64-bit integers to 64-bit floating-point elements.
 *  NOTE: type conversion performed with scalar FPU since SSE does not support 64-bit integer conversions.
 */
static inline SIMD_DBL simd_cvt_u64_f64(const SIMD_INT va) __SIMD_NEED__
{
    uint64_t sa[SIMD_STREAMS_64] SET_ALIGNED(SIMD_WIDTH_BYTES);
    uint64_t *sa_ptr = sa;
    double fa[SIMD_STREAMS_64] SET_ALIGNED(SIMD_WIDTH_BYTES);
    double *fa_ptr = fa; 

    _mm256_store_si256((SIMD_INT *)sa, va);
    for (int32_t i = 0; i < SIMD_STREAMS_64; ++i)
        *(fa_ptr++) = (double)*(sa_ptr++);

    return _mm256_load_pd(fa);
}


/********************
 *  Load intrinsics 
 ********************/
static inline SIMD_INT simd_load(const int32_t * const sa) __SIMD_NEED__
{ return _mm256_load_si256((SIMD_INT *)sa); }

static inline SIMD_INT simd_load(const uint32_t * const sa) __SIMD_NEED__
{ return _mm256_load_si256((SIMD_INT *)sa); }

static inline SIMD_INT simd_load(const int64_t * const sa) __SIMD_NEED__
{ return _mm256_load_si256((SIMD_INT *)sa); }

static inline SIMD_INT simd_load(const uint64_t * const sa) __SIMD_NEED__
{ return _mm256_load_si256((SIMD_INT *)sa); }

static inline SIMD_FLT simd_load(const float * const sa) __SIMD_NEED__ 
{ return _mm256_load_ps(sa); }

static inline SIMD_DBL simd_load(const double * const sa) __SIMD_NEED__
{ return _mm256_load_pd(sa); }


/*******************************
 *  Store intrinsics 
 *******************************/
static inline void simd_store(int32_t * const sa, const SIMD_INT va) __SIMD_NEED__
{ _mm256_store_si256((SIMD_INT *)sa, va); }

static inline void simd_store(uint32_t * const sa, const SIMD_INT va) __SIMD_NEED__
{ _mm256_store_si256((SIMD_INT *)sa, va); }

static inline void simd_store(int64_t * const sa, const SIMD_INT va) __SIMD_NEED__
{ _mm256_store_si256((SIMD_INT *)sa, va); }

static inline void simd_store(uint64_t * const sa, const SIMD_INT va) __SIMD_NEED__
{ _mm256_store_si256((SIMD_INT *)sa, va); }

static inline void simd_store(float * const sa, const SIMD_FLT va) __SIMD_NEED__
{ _mm256_store_ps(sa, va); }

static inline void simd_store(double * const sa, const SIMD_DBL va) __SIMD_NEED__
{ _mm256_store_pd(sa, va); }


#endif  // __AVX2_H

