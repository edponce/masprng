#ifndef __VSPRNG_H
#define __VSPRNG_H


#include "simd.h"
#if defined(SIMD_MODE)


/*!
 *  RNG identifiers
 *  Uses same values as SPRNG interface for convenience user applications.
 */
enum VSPRNG_TYPE
{
    VSPRNG_LFG = 0,
    VSPRNG_LCG,
    VSPRNG_LCG64,
    VSPRNG_CMRG,
    VSPRNG_MLFG,
    VSPRNG_PMLCG
};


/*! \class VSPRNG
 *  \brief Interface (abstract base class) for SIMD RNG types. 
 *
 *  Methods that are virtual require each class for RNG types to define these methods.
 */
class VSPRNG
{
  public:
    virtual ~VSPRNG() {} /*!< virtual destructor allows polymorphism to invoke derived destructors */
    virtual int init_rng(int, int, int * const, int * const) = 0;
    virtual SIMD_INT get_rn_int() const = 0;
    virtual SIMD_FLT get_rn_flt() const = 0;
    virtual SIMD_DBL get_rn_dbl() const = 0;
    virtual SIMD_INT get_seed_rng() const = 0;
    virtual int get_ngens() const = 0;
#if defined(DEBUG)
    virtual SIMD_INT get_prime() const = 0;
    virtual SIMD_INT get_seed() const = 0;
    virtual SIMD_INT get_multiplier() const = 0;
# if defined(LONG_SPRNG)
    virtual SIMD_INT get_seed2() const = 0;
    virtual SIMD_INT get_multiplier2() const = 0;
# endif
#endif
};


#endif // SIMD_MODE


#endif  // __VSPRNG_H

