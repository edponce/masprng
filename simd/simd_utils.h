#ifndef __SIMD_UTILS_H
#define __SIMD_UTILS_H

#include "sse.h"

#if defined(SIMD_MODE)
/**********************************
 *  SIMD general helper functions
 **********************************/
void simd_print(char const * const, SIMD_INT const);
void simd_print(char const * const, SIMD_SP const);
void simd_print(char const * const, SIMD_DP const);
#endif // SIMD_MODE 


#endif // __SIMD_UTILS_H

