#ifndef __LCG_H
#define __LCG_H


#include "sprng.h"
#include "lcg_config.h"


/*! \class LCG 
 *  \brief Class for linear congruential RNG. 
 */
class LCG: public SPRNG
{
  static unsigned int LCG_NGENS;

  public:
    LCG();
    ~LCG();
    int init_rng(int, int);
    int get_rn_int();
    float get_rn_flt();
    double get_rn_dbl();
    int get_seed_rng();

    // NOTE: for debug purposes
    unsigned long int get_seed();
    int get_prime();
    unsigned long int get_multiplier();

  private:
    int rng_type;  /*!< Unique ID for RNG */
    int init_seed;  /*!< Initial seed value */
    int prime;  /*!< Storage for a prime number */
    int prime_position;  /*!< Storage for position of prime number */
    int parameter;

#if defined(LONG_SPRNG)
    unsigned long int seed;  /*!< Seed values calculated using initial seed value */
    unsigned long int multiplier;  /*!< Array for multiplier values */
#else
    int seed[2];  /*!< Pair of seed values calculated using initial seed value */
    int *multiplier;  /*!< Array for multiplier values */
#endif
};


#endif  // __LCG_H

