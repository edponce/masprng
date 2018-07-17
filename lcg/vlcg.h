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
    int init_rng(int, int, const int * const, const int * const, const int = SIMD_STREAMS_32);
    SIMD_INT get_rn_int() const;
    SIMD_FLT get_rn_flt() const;
    SIMD_DBL get_rn_dbl() const;
    SIMD_INT get_seed_rng() const;
    int get_ngens() const;
#if defined(DEBUG)
    SIMD_INT get_prime() const;
    SIMD_INT get_seed() const;
    SIMD_INT get_multiplier() const;
# if defined(LONG_SPRNG)
    SIMD_INT get_seed2() const;
    SIMD_INT get_multiplier2() const;
# endif
#endif

  private:
    const char *gentype;
    int32_t rng_type;
    int32_t prime_position;
    int32_t prime_next;
    SIMD_INT *init_seed;
    SIMD_INT *parameter;
    SIMD_INT *prime;
    SIMD_INT *seed;
    SIMD_INT *multiplier;
    SIMD_INT *strm_mask32;
    SIMD_INT *strm_mask64;
    void multiply(SIMD_INT * const, const SIMD_INT * const, const SIMD_INT * const) const;
} __SIMD_ALIGN__;


#endif // SIMD_MODE


#endif  // __VLCG_H

