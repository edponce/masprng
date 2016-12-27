#ifndef __SPRNG_H
#define __SPRNG_H


/*
 *  Check for SIMD mode
 */
#if defined(USE_SSE)
#include "sse.h"
#define SIMD_MODE /* MASPRNG vector flag */
#else
/*
 *  Scalar mode
 *  NOTE: need to rework this, rename macros
 */
#define SIMD_NUM_STREAMS 1
#define SIMD_ALIGN 8
#endif


/*
 *  RNG identifiers
 */
#define SPRNG_LFG   0
#define SPRNG_LCG   1
#define SPRNG_LCG64 2
#define SPRNG_CMRG  3
#define SPRNG_MLFG  4
#define SPRNG_PMLCG 5


/*! \class SPRNG
 *  \brief Interface (abstract base class) for RNG types. 
 *
 *  Methods that are virtual require each class for RNG types to define these methods.
 */
class SPRNG
{
  public:
    virtual ~SPRNG() {} /*!< virtual destructor allows polymorphism to invoke derived destructors */
    virtual int init_rng(int, int) = 0;
    virtual int get_rn_int() = 0;
    virtual float get_rn_flt() = 0;
    virtual double get_rn_dbl() = 0;
    virtual int get_seed_rng() = 0;

    // NOTE: for debug purposes
    virtual int get_prime() = 0;
#if defined(LONG_SPRNG)
    virtual unsigned long int get_seed() = 0;
    virtual unsigned long int get_multiplier() = 0;
#else
    virtual int get_seed() = 0;
    virtual int get_multiplier() = 0;
#endif
};


/*! \class VSPRNG
 *  \brief Interface (abstract base class) for SIMD RNG types. 
 *
 *  Methods that are virtual require each class for RNG types to define these methods.
 */
#if defined(SIMD_MODE)
class VSPRNG
{
  public:
    virtual ~VSPRNG() {} /*!< virtual destructor allows polymorphism to invoke derived destructors */
    virtual int init_rng(int *, int *) = 0;
    virtual SIMD_INT get_rn_int() = 0;
    virtual SIMD_FLT get_rn_flt() = 0;
    virtual SIMD_DBL get_rn_dbl() = 0;
    virtual SIMD_INT get_seed_rng() = 0;

    // NOTE: for debug purposes
    virtual SIMD_INT get_seed() = 0;
    virtual SIMD_INT get_prime() = 0;
    virtual SIMD_INT get_multiplier() = 0;
};
#endif


#endif  // __SPRNG_H

