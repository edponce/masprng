#ifndef __SIMD_H
#define __SIMD_H


/*
 *  Compiler and architecture specific settings 
 */
#include "arch.h"


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
#endif


/*
 *  (Mandatory) Alignment macro.
 *  Uses SIMD_WIDTH_BYTES provided by SIMD modules.
 */
#if defined(SIMD_MODE)
    #include <stdint.h>
    #define __SIMD_ALIGNED SET_ALIGNED(SIMD_WIDTH_BYTES)
#else
    #define __SIMD_ALIGNED // disable for scalar mode
#endif


#endif  // __SIMD_H

