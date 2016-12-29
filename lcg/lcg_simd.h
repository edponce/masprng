#ifndef __LCG_SIMD_H
#define __LCG_SIMD_H


#include "sprng.h"


#if defined(SIMD_MODE)
/*! \class VLCG 
 *  \brief Class for SIMD linear congruential RNG.
 */
class VLCG: public VSPRNG
{
  // NOTE: thread-safe?
  static unsigned long int LCG_NGENS;

  public:
    VLCG();
    ~VLCG();
    int init_rng(int, int, int * const, int * const);
    SIMD_INT get_rn_int();
    SIMD_FLT get_rn_flt();
    SIMD_DBL get_rn_dbl();
    SIMD_INT get_seed_rng() const;
    unsigned long int get_ngens() const;
#if defined(DEBUG)
    SIMD_INT get_seed() const;
    SIMD_INT get_prime() const;
    SIMD_INT get_multiplier() const;
#endif

  private:
    int rng_type;
    SIMD_INT init_seed;
    SIMD_INT prime;
    SIMD_INT prime_position;
    SIMD_INT prime_next;
    SIMD_INT parameter;
#if defined(LONG_SPRNG)
    SIMD_INT seed;
    SIMD_INT multiplier;
    SIMD_INT multiply(const SIMD_INT, const SIMD_INT, const SIMD_INT) const;
#else
    SIMD_INT seed[2];
    SIMD_INT multiplier[4];
    void multiply(SIMD_INT * const, SIMD_INT * const, const SIMD_INT) const;
#endif
    const char *gentype;
    
    // SIMD masks
    void init_simd_masks();
    SIMD_INT vmsk_lsb1[SIMD_NUM_STREAMS+1];
    SIMD_INT vmsk_lh64[SIMD_NUM_STREAMS+1];
    SIMD_INT vmsk_hi64;
    SIMD_INT vmsk_lsb24;
    SIMD_INT vmsk_lsb48;
    SIMD_INT vmsk_lsb31;
    SIMD_INT vmsk_msbset64;
    SIMD_INT vmsk_msbset24;
};
#endif  // SIMD_MODE


#endif  // __LCG_SIMD_H

