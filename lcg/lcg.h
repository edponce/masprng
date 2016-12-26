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
    int rng_type;
    int init_seed;
    int prime;
    int prime_position;
    int parameter;

#if defined(LONG_SPRNG)
    unsigned long int seed;
    unsigned long int multiplier;
#else
    int seed[2];
    int *multiplier;
#endif
};


#endif  // __LCG_H

