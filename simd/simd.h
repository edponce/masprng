#ifndef __SIMD_H
#define __SIMD_H


/*
 *  If SIMD_MODE is enabled, use compiler flags to
 *  check automatically for best SIMD mode supported.
 */
#if defined(SIMD_MODE)
    #undef SIMD_MODE
    #undef AVX512_SPRNG
    #undef AVX2_SPRNG
    #undef SSE4_1_SPRNG
# if defined(__AVX512BW__)
    #define AVX512_SPRNG
# elif defined(__AVX2__)
    #define AVX2_SPRNG
# elif defined(__SSE4_1__)
    #define SSE4_1_SPRNG
# endif
#endif


/*
 *  If SIMD_MODE is disabled, check for the SIMD mode requested.
 *  SIMD_MODE has to be defined to access available SIMD features.
 *  Include corresponding SIMD interfaces.
 */
#if defined(AVX512_SPRNG)
    #define SIMD_MODE
    #include "avx512.h"
#elif defined(AVX2_SPRNG)
    #define SIMD_MODE
    #include "avx2.h"
#elif defined(SSE4_1_SPRNG)
    #define SIMD_MODE
    #include "sse4_1.h"
#else
    // Disable SIMD for scalar mode
    #define SIMD_WIDTH_BYTES 8
    #define SIMD_STREAMS_32  1
    #define SIMD_STREAMS_64  1
#endif


/*
 *  Compiler and architecture specific settings 
 */
#include "arch.h"


/*
 *  Alignment macros
 *  Use SIMD_WIDTH_BYTES provided by SIMD modules and
 *  macros provided by compiler/architecture settings.
 */
#define __SIMD_ALIGN__             ARCH_ATTR_ALIGNED(SIMD_WIDTH_BYTES)
#define __SIMD_ASSUME_ALIGNED__(a) ARCH_ASSUME_ALIGNED(a, SIMD_WIDTH_BYTES)
#define __SIMD_ASSUME__(a)         ARCH_ASSUME(a)


#endif  // __SIMD_H

