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
    int get_prime() const;
  #if defined(LONG_SPRNG)
    unsigned long int get_seed() const;
    unsigned long int get_multiplier() const;
  #else
    int get_seed() const;
    int get_multiplier() const;
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
    unsigned long int multiply(const unsigned long int, const unsigned long int, const unsigned long int) const;
#else
    int seed[2];
    int multiplier[4];
    void multiply(int * const, const int * const, const int) const;
#endif
};


#endif  // __LCG_H

