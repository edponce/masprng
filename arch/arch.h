#ifndef __ARCH_H
#define __ARCH_H


// Compiler specific macro to set array alignment.
#if defined(__GNUC__)
    #define SET_ALIGNED(a) __attribute__((aligned(a)))
    #define ASSUME_ALIGNED(a, x)
    #define ASSUME(a) __builtin_assume(a)
#elif defined(__INTEL_COMPILER)
    #define SET_ALIGNED(a) __attribute__((aligned(a)))
    #define ASSUME_ALIGNED(a, x) __assume_aligned(a, x)
    #define ASSUME(a) __assume(a)
#else
    #define SET_ALIGNED(a)
    #define ASSUME_ALIGNED(a, x)
    #define ASSUME(a)
#endif


#endif  // __ARCH_H

