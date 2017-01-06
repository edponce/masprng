#ifndef __SPRNG_H
#define __SPRNG_H


#include "masprng_config.h"


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
# if defined(LONG_SPRNG)
    virtual unsigned long int get_seed() const = 0;
    virtual unsigned long int get_multiplier() const = 0;
# else
    virtual int get_seed() const = 0;
    virtual int get_multiplier() const = 0;
# endif
#endif
};


#endif  // __SPRNG_H

