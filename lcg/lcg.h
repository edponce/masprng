#ifndef __LCG_H
#define __LCG_H


// Constants from lcg.cpp
#define INIT_SEED 0x2bc68cfe166dL
#define MSB_SET 0x3ff0000000000000L
#define LSB48 0xffffffffffffL
#define AN1 0xdadf0ac00001L
#define AN2 0xfefd7a400001L
#define AN3 0x6417b5c00001L
#define AN4 0xcf9f72c00001L
#define AN5 0xbdf07b400001L
#define AN6 0xf33747c00001L
#define AN7 0xcbe632c00001L
#define PMULT1 0xa42c22700000L
#define PMULT2 0xfa858cb00000L
#define PMULT3 0xd0c4ef00000L
#define PMULT4 0xc3cc8e300000L
#define PMULT5 0x11bdbe700000L
#define PMULT6 0xb0f0e9f00000L
#define PMULT7 0x6407de700000L
#define MULT1 0x2875a2e7b175L   /* 44485709377909  */
#define MULT2 0x5deece66dL      /* 1575931494      */
#define MULT3 0x3eac44605265L   /* 68909602460261  */
#define MULT4 0x275b38eb4bbdL   /* 4327250451645   */
#define MULT5 0x1ee1429cc9f5L   /* 33952834046453  */
#define MULT6 0x739a9cb08605L   /* 127107890972165 */
#define MULT7 0x3228d7cc25f5L   /* 55151000561141  */

#define TWO_M24 5.96046447753906234e-8
#define TWO_M48 3.5527136788005008323e-15

#define LCGRUNUP 29

#if defined(LittleEndian)
#define MSB 1
#else
#define MSB 0
#endif
#define LSB (1-MSB)

#define RNG_LONG64_DBL 3.5527136788005008e-15
#define MAX_MULTIPLIERS 7


// Scalar mode
int init_rng(unsigned long int *, unsigned long int *, unsigned int *, int, int);
int get_rn_int(unsigned long int *, unsigned long int, unsigned int);
float get_rn_flt(unsigned long int *, unsigned long int, unsigned int);
double get_rn_dbl(unsigned long int *, unsigned long int, unsigned int);


// SIMD mode
#if defined(SIMD_MODE)
int init_vrng(VECTOR_INT *, VECTOR_INT *, VECTOR_INT *, int *, int *);
VECTOR_INT get_vrn_int(VECTOR_INT *, const VECTOR_INT , const VECTOR_INT);
VECTOR_SP get_vrn_flt(VECTOR_INT *, const VECTOR_INT , const VECTOR_INT);
VECTOR_DP get_vrn_dbl(VECTOR_INT *, const VECTOR_INT , const VECTOR_INT);
#endif


#endif  // __LCG_H

