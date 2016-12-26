#ifndef __LCG_SIMD_H
#define __LCG_SIMD_H


#include "sprng.h"
#include "lcg_config.h"


/*! \class VLCG 
 *  \brief Class for SIMD linear congruential RNG. 
 */
#if defined(SIMD_MODE)
class VLCG: public VSPRNG
{
  static unsigned int LCG_NGENS;

  public:
    VLCG();
    ~VLCG();
    int init_rng(int *, int *); // NOTE: initialize using SIMD types?
    SIMD_INT get_rn_int();
    SIMD_SP get_rn_flt();
    SIMD_DP get_rn_dbl();
    SIMD_INT get_seed_rng();

    // NOTE: for debug purposes
    SIMD_INT get_seed();
    SIMD_INT get_prime();
    SIMD_INT get_multiplier();

  private:
    int rng_type;  /*!< Unique ID for RNG */
    SIMD_INT init_seed;  /*!< Initial seed value */
    SIMD_INT prime;  /*!< Storage for a prime number */
    SIMD_INT prime_position;  /*!< Storage for position of prime number */
    SIMD_INT parameter;
    SIMD_INT seed;  /*!< Seed values calculated using initial seed value */
    SIMD_INT multiplier;  /*!< Array for multiplier values */
};
#endif


#endif  // __LCG_SIMD_H

