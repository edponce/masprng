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
    static int LCG_NGENS;

  public:
    VLCG();
    ~VLCG();
    int init_rng(int, int, int * const, int * const);
    SIMD_INT get_rn_int();
    SIMD_FLT get_rn_flt();
    SIMD_DBL get_rn_dbl();
    SIMD_INT get_seed_rng() const;
    int get_ngens() const;
#if defined(DEBUG)
    SIMD_INT get_seed() const;
    SIMD_INT get_prime() const;
    SIMD_INT get_multiplier() const;
#endif

  private:
    const char *gentype;    
    int32_t rng_type;
    int32_t prime_position;
    int32_t prime_next;
#if defined(LONG_SPRNG)
    SIMD_INT init_seed[2] __SIMD_ALIGNED;
    SIMD_INT parameter[2] __SIMD_ALIGNED;
    SIMD_INT prime[2] __SIMD_ALIGNED;
    SIMD_INT seed[2] __SIMD_ALIGNED;
    SIMD_INT multiplier[2];
    SIMD_INT multiply(const SIMD_INT, const SIMD_INT, const SIMD_INT) const;
#else
    SIMD_INT init_seed;
    SIMD_INT parameter;
    SIMD_INT prime;
    SIMD_INT seed[2] __SIMD_ALIGNED;
    SIMD_INT multiplier[4] __SIMD_ALIGNED;
    void multiply(SIMD_INT * const, SIMD_INT * const, const SIMD_INT) const;
#endif
};


#endif // SIMD_MODE


#endif  // __VLCG_H

