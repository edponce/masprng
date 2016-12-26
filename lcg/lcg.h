#ifndef __LCG_H
#define __LCG_H


#include "sprng.h"


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

    // NOTE: for debug purposes
    unsigned long int get_seed();
    int get_prime();
    unsigned long int get_multiplier();

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
};


/*! \class VLCG 
 *  \brief Class for SIMD linear congruential RNG. 
 */
#ifdef SIMD_MODE
class VLCG: public VSPRNG
{
  static unsigned int LCG_NGENS;

  public:
    VLCG();
    ~VLCG();
    int init_vrng(int *, int *); // NOTE: initialize using SIMD types?
    SIMD_INT get_vrn_int();
    SIMD_SP get_vrn_flt();
    SIMD_DP get_vrn_dbl();

    // NOTE: for debug purposes
    SIMD_INT get_vseed();
    SIMD_INT get_vprime();
    SIMD_INT get_vmultiplier();

  private:
    int rng_type;  /*!< Unique ID for RNG */
#ifdef LONG_SPRNG
    SIMD_INT vseed;  /*!< Seed values calculated using initial seed value */
    SIMD_INT vprime;  /*!< Storage for a prime number */
    SIMD_INT vmultiplier;  /*!< Array for multiplier values */
#endif
};
#endif


#endif  // __LCG_H

