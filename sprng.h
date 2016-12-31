#ifndef __SPRNG_H
#define __SPRNG_H


/*
 *  Check for SIMD mode
 */
#if defined(SSE_SPRNG)
#define SIMD_MODE 1 /*!< NOTE: mandatory, MASPRNG vector flag */
#include "sse.h"
#endif


// Global constants
const int SIMD_ALIGN = SIMD_WIDTH_BYTES;
const int SIMD_STREAMS_INT = (SIMD_WIDTH_BYTES/sizeof(int));
const int SIMD_STREAMS_FLT = (SIMD_WIDTH_BYTES/sizeof(float));
const int SIMD_STREAMS_DBL = (SIMD_WIDTH_BYTES/sizeof(double));

// NOTE: need to remove this variable
const int SIMD_NUM_STREAMS = (SIMD_WIDTH_BYTES/sizeof(double));



/*!
 *  RNG identifiers
 */
enum SPRNG_TYPE {SPRNG_LFG = 0, SPRNG_LCG, SPRNG_LCG64, SPRNG_CMRG, SPRNG_MLFG, SPRNG_PMLCG};


/*! \class SPRNG
 *  \brief Interface (pure abstract class) used as base class for RNG types. 
 *
 *  Methods that are virtual require each derived class to define these methods.
 */
class SPRNG
{
  public:
    virtual ~SPRNG() {} /*!< virtual destructor allows polymorphism to invoke derived destructors */
    virtual int init_rng(int, int, int, int) = 0;
    virtual int get_rn_int() = 0;
    virtual float get_rn_flt() = 0;
    virtual double get_rn_dbl() = 0;
    virtual int get_seed_rng() const = 0;
    virtual unsigned long int get_ngens() const = 0;
#if defined(DEBUG)
    virtual int get_prime() const = 0;
#if defined(LONG_SPRNG)
    virtual unsigned long int get_seed() const = 0;
    virtual unsigned long int get_multiplier() const = 0;
#else
    virtual int get_seed() const = 0;
    virtual int get_multiplier() const = 0;
#endif
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
    virtual int init_rng(int, int, int * const , int * const) = 0;
    virtual SIMD_INT get_rn_int() = 0;
    virtual SIMD_FLT get_rn_flt() = 0;
    virtual SIMD_DBL get_rn_dbl() = 0;
    virtual SIMD_INT get_seed_rng() const = 0;
    virtual unsigned long int get_ngens() const = 0;
#if defined(DEBUG)
    virtual SIMD_INT get_seed() const = 0;
    virtual SIMD_INT get_prime() const = 0;
    virtual SIMD_INT get_multiplier() const = 0;
#endif
};
#endif


#endif  // __SPRNG_H

