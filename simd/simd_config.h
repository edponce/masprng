#ifndef __SIMD_CONFIG_H
#define __SIMD_CONFIG_H


/*
 *  If SIMD_MODE is enabled, use compiler flags to
 *  check automatically for best SIMD mode supported.
 */
#if defined(SIMD_MODE)
    #undef AVX512_SPRNG
    #undef AVX2_SPRNG
    #undef SSE4_1_SPRNG
# if defined(__AVX512BW__)
    #define AVX512_SPRNG
# elif defined(__AVX2__)
    #define AVX2_SPRNG
# elif defined(__SSE4_1__)
    #define SSE4_1_SPRNG
# else
    #undef SIMD_MODE
# endif
#endif


/*
 *  Include corresponding SIMD interfaces.
 *  If SIMD_MODE is disabled, check for the SIMD mode requested.
 */
#if defined(AVX512_SPRNG)
    #define SIMD_MODE 1 /*!< Mandatory global SIMD flag */
    #include "avx512.h"
#elif defined(AVX2_SPRNG)
    #define SIMD_MODE 1 /*!< Mandatory global SIMD flag */
    #include "avx2.h"
#elif defined(SSE4_1_SPRNG)
    #define SIMD_MODE 1 /*!< Mandatory global SIMD flag */
    #include "sse4_1.h"
#endif


/*
 *  Global SIMD constants for alignment and streams counts
 */
#if defined(SIMD_MODE)
const int SIMD_ALIGN = SIMD_WIDTH_BYTES;
const int SIMD_STREAMS_INT = (SIMD_WIDTH_BYTES/sizeof(int));
const int SIMD_STREAMS_FLT = (SIMD_WIDTH_BYTES/sizeof(float));
const int SIMD_STREAMS_DBL = (SIMD_WIDTH_BYTES/sizeof(double));
#else
const int SIMD_ALIGN = 8;  // NOTE: default alignment is 64-bit architecture
const int SIMD_STREAMS_INT = 1;
const int SIMD_STREAMS_FLT = 1;
const int SIMD_STREAMS_DBL = 1;
#endif


#endif  // __SIMD_CONFIG_H

