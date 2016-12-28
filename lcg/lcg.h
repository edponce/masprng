#ifndef __LCG_H
#define __LCG_H


#include "lcg_config.h"
#include "sprng.h"


/*! \class LCG 
 *  \brief Class for linear congruential RNG. 
 */
class LCG: public SPRNG
{
  static unsigned long int LCG_NGENS;

  public:
    LCG();
    ~LCG();
    int init_rng(int, int, int, int);
    int get_rn_int();
    float get_rn_flt();
    double get_rn_dbl();
    int get_seed_rng() const;
    unsigned long int get_ngens() const;

    // NOTE: for debug purposes
    int get_prime();
#if defined(LONG_SPRNG)
    unsigned long int get_seed();
    unsigned long int get_multiplier();
#else
    int get_seed();
    int get_multiplier();
#endif

  private:
    int rng_type;
    int init_seed;
    int prime;
    int prime_position;
    int prime_next;
    int parameter;
    const char *gentype;

#if defined(LONG_SPRNG)
    unsigned long int seed;
    unsigned long int multiplier;

    // NOTE: need to rename
    unsigned long int mults_g[7];
    unsigned long int multiplier_g;
#else
    int seed[2];
    int *multiplier;
    int mults_g[7][4];
    int *multiplier_g;
#endif

    unsigned long int multiply_48_64(unsigned long int, int) const;
};


#endif  // __LCG_H

