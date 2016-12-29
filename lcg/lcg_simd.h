#ifndef __LCG_SIMD_H
#define __LCG_SIMD_H


#include "sprng.h"


#if defined(SIMD_MODE)
/*! \class VLCG 
 *  \brief Class for SIMD linear congruential RNG.
 */
class VLCG: public VSPRNG
{
  // NOTE: not thread-safe?
  static unsigned long int LCG_NGENS;

  public:
    VLCG();
    ~VLCG();
    int init_rng(int, int, int *, int *);
    SIMD_INT get_rn_int();
    SIMD_FLT get_rn_flt();
    SIMD_DBL get_rn_dbl();
    SIMD_INT get_seed_rng() const;
    unsigned long int get_ngens() const;
    // NOTE: for debug purposes
    SIMD_INT get_seed();
    SIMD_INT get_prime();
    SIMD_INT get_multiplier();

  private:
    int rng_type;
    SIMD_INT init_seed;
    SIMD_INT prime;
    SIMD_INT prime_position;
    SIMD_INT prime_next;
    SIMD_INT parameter;
    SIMD_INT seed;
    SIMD_INT multiplier;
    const char *gentype;
#if defined(LONG_SPRNG)
    // NOTE: need to rename
    unsigned long int mults_g[7];
    unsigned long int multiplier_g;
#else
    int mults_g[7][4];
    int *multiplier_g;
#endif
    // SIMD masks
    SIMD_INT vmsk_lsb1[SIMD_NUM_STREAMS+1];
    SIMD_INT vmsk_lh64[SIMD_NUM_STREAMS+1];
    SIMD_INT vmsk_hi64;
    SIMD_INT vmsk_lsb48;
    SIMD_INT vmsk_seed;

    void init_simd_masks();
    SIMD_INT multiply(SIMD_INT, SIMD_INT, SIMD_INT) const;
};
#endif  // SIMD_MODE


#endif  // __LCG_SIMD_H

