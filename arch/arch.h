#ifndef __ARCH_H
#define __ARCH_H


/*
 *  Compiler specific macro to set array alignment
 */
#if defined(__GNUC__) || defined(__INTEL_COMPILER)
    #define SET_ALIGNED(a) __attribute__((aligned(a)))
#else
    #define SET_ALIGNED(a)
#endif


#endif  // __ARCH_H

