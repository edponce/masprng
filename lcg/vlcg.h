#ifndef __VLCG_H
#define __VLCG_H


#include "simd.h"
#if defined SIMD_MODE


#include "vsprng.h"


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
};


#endif // SIMD_MODE


#endif  // __VLCG_H

