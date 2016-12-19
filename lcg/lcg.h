#ifndef __LCG_H
#define __LCG_H


/*
 *  Scalar mode
 */
int init_rng(unsigned long int *, unsigned long int *, unsigned int *, int, int);
int get_rn_int(unsigned long int *, unsigned long int, unsigned int);
float get_rn_flt(unsigned long int *, unsigned long int, unsigned int);
double get_rn_dbl(unsigned long int *, unsigned long int, unsigned int);


/*
 *  SIMD mode
 */
#if defined(USE_SSE)
#include "sse.h"
#define SIMD_MODE /* MASPRNG vector flag */

int init_vrng(SIMD_INT *, SIMD_INT *, SIMD_INT *, int *, int *);
SIMD_INT get_vrn_int(SIMD_INT *, const SIMD_INT , const SIMD_INT);
SIMD_SP get_vrn_flt(SIMD_INT *, const SIMD_INT , const SIMD_INT);
SIMD_DP get_vrn_dbl(SIMD_INT *, const SIMD_INT , const SIMD_INT);
#endif


#endif  // __LCG_H

