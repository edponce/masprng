#ifndef __LCG_H
#define __LCG_H


#include "sprng.h"


/*! \class LCG 
 *  \brief Class for linear congruential RNG. 
 */
class LCG: public SPRNG
{
  public:
    static unsigned int LCG_NGENS;
    LCG();
    ~LCG();
    int init_rng(int, int);
    int get_rn_int();
    float get_rn_flt();
    double get_rn_dbl();

    // NOTE: for debug purposes
    unsigned long int get_seed();
    int get_prime();
    unsigned long int get_multiplier();

#ifdef SIMD_MODE
    int init_vrng(int, int);
    SIMD_INT get_vrn_int();
    SIMD_SP get_vrn_flt();
    SIMD_DP get_vrn_dbl();
#endif

  private:
    int rng_type;  /*!< Unique ID for RNG */
#ifdef LONG_SPRNG
    unsigned long int seed;  /*!< Seed values calculated using initial seed value */
    int init_seed;  /*!< Initial seed value */
    int prime;  /*!< Storage for a prime number */
    unsigned long int multiplier;  /*!< Array for multiplier values */
#else
    int seed[2];  /*!< Pair of seed values calculated using initial seed value */
    int init_seed;  /*!< Initial seed value */
    int prime;  /*!< Storage for a prime number */
    int *multiplier;  /*!< Array for multiplier values */
#endif

#ifdef SIMD_MODE
    SIMD_INT seed;  /*!< Seed values calculated using initial seed value */
    SIMD_INT prime;  /*!< Storage for a prime number */
    SIMD_INT multiplier;  /*!< Array for multiplier values */
#endif
};


#endif  // __LCG_H

