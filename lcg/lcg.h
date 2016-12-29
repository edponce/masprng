#ifndef __LCG_H
#define __LCG_H


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
#if defined(DEBUG)
    int get_prime();
#if defined(LONG_SPRNG)
    unsigned long int get_seed();
    unsigned long int get_multiplier();
#else
    int get_seed();
    int get_multiplier();
#endif
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

    unsigned long int multiply(unsigned long int, unsigned long int, unsigned long int) const;
#else
    int seed[2];
    int *multiplier;

    unsigned long int multiply(unsigned long int, unsigned long int, unsigned long int) const;
#endif
};


#endif  // __LCG_H

