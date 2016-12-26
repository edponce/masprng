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
    int init_seed;  /*!< Initial seed value */
    int prime;  /*!< Storage for a prime number */
    int prime_position;  /*!< Storage for position of prime number */
    int parameter;

#ifdef LONG_SPRNG
    unsigned long int seed;  /*!< Seed values calculated using initial seed value */
    unsigned long int multiplier;  /*!< Array for multiplier values */
#else
    int seed[2];  /*!< Pair of seed values calculated using initial seed value */
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
    int init_rng(int *, int *); // NOTE: initialize using SIMD types?
    SIMD_INT get_rn_int();
    SIMD_SP get_rn_flt();
    SIMD_DP get_rn_dbl();

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


#endif  // __LCG_H

