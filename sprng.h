#ifndef __SPRNG_H
#define __SPRNG_H


/*
 *  Check for SIMD mode
 */
#if defined(USE_SSE)
#define SIMD_MODE 1 /*!< (NOTE: mandatory) MASPRNG vector flag */
#include "sse.h"

#else
/*
 *  Scalar mode
 */
#if defined(LONG_SPRNG)
#define SIMD_WIDTH_BYTES 8
#else
#define SIMD_WIDTH_BYTES 4
#endif
#endif

#define SIMD_ALIGN SIMD_WIDTH_BYTES
#if defined(LONG_SPRNG)
#define SIMD_NUM_STREAMS (SIMD_WIDTH_BYTES/8)
#else
#define SIMD_NUM_STREAMS (SIMD_WIDTH_BYTES/4)
#endif


/*!
 *  RNG identifiers
 */
enum SPRNG_CONFIG {SPRNG_LFG = 0, SPRNG_LCG, SPRNG_LCG64, SPRNG_CMRG, SPRNG_MLFG, SPRNG_PMLCG};


/*! \class SPRNG
 *  \brief Interface (abstract base class) for RNG types. 
 *
 *  Methods that are virtual require each class for RNG types to define these methods.
 */
class SPRNG
{
  public:
    virtual ~SPRNG() {} /*!< virtual destructor allows polymorphism to invoke derived destructors */
    virtual int init_rng(int, int, int, int) = 0;
    virtual int get_rn_int() = 0;
    virtual float get_rn_flt() = 0;
    virtual double get_rn_dbl() = 0;
    virtual int get_seed_rng() = 0;
    virtual unsigned long int get_ngens() = 0;

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


#if defined(SIMD_MODE)
/*! \class VSPRNG
 *  \brief Interface (abstract base class) for SIMD RNG types. 
 *
 *  Methods that are virtual require each class for RNG types to define these methods.
 */
class VSPRNG
{
  public:
    virtual ~VSPRNG() {} /*!< virtual destructor allows polymorphism to invoke derived destructors */
    virtual int init_rng(int, int, int *, int *) = 0;
    virtual SIMD_INT get_rn_int() = 0;
    virtual SIMD_FLT get_rn_flt() = 0;
    virtual SIMD_DBL get_rn_dbl() = 0;
    virtual SIMD_INT get_seed_rng() = 0;
    virtual unsigned long int get_ngens() = 0;

    // NOTE: for debug purposes
    virtual SIMD_INT get_seed() = 0;
    virtual SIMD_INT get_prime() = 0;
    virtual SIMD_INT get_multiplier() = 0;
};
#endif


#endif  // __SPRNG_H

