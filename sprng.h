#ifndef __SPRNG_H
#define __SPRNG_H


/*
 *  SIMD mode
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
    virtual ~SPRNG() {} /* virtual destructor allows polymorphism to invoke derived destructors */
    virtual int init_rng(int, int) = 0;
    virtual int get_rn_int() = 0;
    virtual float get_rn_flt() = 0;
    virtual double get_rn_dbl() = 0;
    virtual int get_seed_rng() = 0;
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
    virtual ~VSPRNG() {} /* virtual destructor allows polymorphism to invoke derived destructors */
    virtual int init_rng(int *, int *) = 0;
    virtual SIMD_INT get_rn_int() = 0;
    virtual SIMD_SP get_rn_flt() = 0;
    virtual SIMD_DP get_rn_dbl() = 0;
    virtual SIMD_INT get_seed_rng() = 0;
};
#endif


#endif  // __SPRNG_H

