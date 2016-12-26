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
};


/*! \class VSPRNG
 *  \brief Interface (abstract base class) for SIMD RNG types. 
 *
 *  Methods that are virtual require each class for RNG types to define these methods.
 */
#ifdef SIMD_MODE
class VSPRNG
{
  public:
    virtual ~VSPRNG() {} /* virtual destructor allows polymorphism to invoke derived destructors */
    virtual int init_vrng(int *, int *) = 0;
    virtual SIMD_INT get_vrn_int() = 0;
    virtual SIMD_SP get_vrn_flt() = 0;
    virtual SIMD_DP get_vrn_dbl() = 0;
};
#endif


#endif  // __SPRNG_H

