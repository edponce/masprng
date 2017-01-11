#ifndef __VUTILS_H
#define __VUTILS_H


#include "simd.h"
#if defined(SIMD_MODE)


int scalar_malloc(int ** const, const size_t, const size_t);
int scalar_malloc(unsigned int ** const, const size_t, const size_t);
int scalar_malloc(long int ** const, const size_t, const size_t);
int scalar_malloc(unsigned long int ** const, const size_t, const size_t);
int scalar_malloc(float ** const, const size_t, const size_t);
int scalar_malloc(double ** const, const size_t, const size_t);
int simd_malloc(SIMD_INT ** const, const size_t, const size_t);
int simd_malloc(SIMD_FLT ** const, const size_t, const size_t);
int simd_malloc(SIMD_DBL ** const, const size_t, const size_t);
void simd_print(const char * const, const SIMD_INT);
void simd_print(const char * const, const SIMD_FLT);
void simd_print(const char * const, const SIMD_DBL);


#endif // SIMD_MODE


#endif  // __VUTILS_H

