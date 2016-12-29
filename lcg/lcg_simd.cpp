/*************************************************************************/
/*************************************************************************/
/*             Parallel 48 bit Linear Congruential Generator             */
/*                                                                       */ 
/* Modified by: Eduardo Ponce                                            */
/*             The University of Tennessee, Knoxville                    */
/*             Email: eponcemo@utk.edu (Jan 2017)                        */
/*                                                                       */
/* Based on the implementation by:                                       */
/*             J. Ren                                                    */
/*             Ashok Srinivasan (Apr 13, 1998)                           */
/*************************************************************************/
/*************************************************************************/


#include "lcg_simd.h"
#if defined(SIMD_MODE)


#include <stdio.h>
#include <limits.h>
#include "primes_32.h"
#include "lcg_config.h"


unsigned long int VLCG::LCG_NGENS = 0;


VLCG::VLCG()
{
    rng_type = SPRNG_LCG;
    simd_set_zero(&init_seed);
    simd_set_zero(&prime);
    simd_set_zero(&prime_position);
    simd_set_zero(&prime_next);
    simd_set_zero(&parameter);
    simd_set_zero(&multiplier);
    gentype = CONFIG.GENTYPE; // NOTE: refactor this, use std::string or char[] with strncpy

#if defined(LONG_SPRNG) 
    seed = simd_set(CONFIG.INIT_SEED, CONFIG.INIT_SEED);
#else
    seed = simd_set(CONFIG.INIT_SEED[1], CONFIG.INIT_SEED[0]);
#endif

    init_simd_masks();

    ++LCG_NGENS;
}


VLCG::~VLCG()
{
    --LCG_NGENS;
}


/*!
 *  Initialize global SIMD masks
 */
void VLCG::init_simd_masks()
{
    // Set vector masks
    vmsk_lsb1[0] = simd_set(0x1UL);                         // all 
    vmsk_lsb1[1] = simd_set(0x0UL, 0x1UL);                // first
    vmsk_lsb1[2] = simd_set(0x1UL, 0x0UL);                // second
    vmsk_lh64[0] = simd_set(0xFFFFFFFFFFFFFFFFUL);         // all 
    vmsk_lh64[1] = simd_set(0x0UL, 0xFFFFFFFFFFFFFFFFUL); // first
    vmsk_lh64[2] = simd_set(0xFFFFFFFFFFFFFFFFUL, 0x0UL); // second
    vmsk_hi64 = simd_set(0xFFFFFFFF00000000UL);            // all
    vmsk_lsb48 = simd_set(CONFIG.LSB48);                    // all
    vmsk_seed = simd_set(0x7FFFFFFFUL);                    // only 31 LSB of seed considered
}


#if defined(LONG_SPRNG)
/*!
 *  LCG multiply 48-bits using 64-bits.
 */
SIMD_INT VLCG::multiply(const SIMD_INT a, const SIMD_INT b, const SIMD_INT c) const
{
    SIMD_INT res = a;
    res = simd_mul_u64(res, b);
    res = simd_add_64(res, c);
    res = simd_and(res, vmsk_lsb48);
    return res;
}
#else
SIMD_INT VLCG::multiply(const SIMD_INT a, const SIMD_INT b, const SIMD_INT c) const
{
    SIMD_INT res = a;
    res = simd_mul_u64(res, b);
    res = simd_add_64(res, c);
    res = simd_and(res, vmsk_lsb48);
    return res;
}
#endif


/*!
 *  Initialize SIMD RNG
 *  The gn parameter is the same for all streams since the generators are run
 *  for awhile based on gn. This also applies to tg parameter.
 */
int VLCG::init_rng(int gn, int tg, int *s, int *m)
{
    int i;
    int need = 1;
    int lprime[SIMD_NUM_STREAMS];
    SIMD_INT vs;

    if (tg <= 0) {
        printf("ERROR: total_gen out of range, %d\n", tg);
        tg = 1;
    }

    if (gn < 0 || gn >= tg || gn >= CONFIG.LCG_MAX_STREAMS) {
        printf("ERROR: gennum out of range, %d\n", gn);
        return -1;
    }

    for (i = 0; i < SIMD_NUM_STREAMS; ++i) {
        getprime_32(need, &lprime[i], gn);

        if (m[i] < 0 || m[i] >= CONFIG.NPARAMS) {
            printf("ERROR: multiplier out of range, %d\n", m[i]);
            m[i] = 0;
        }
    }

    vs = simd_set(s[1], s[0]);
    vs = simd_and(vs, vmsk_seed);
    init_seed = vs;

    prime = simd_set(lprime[1], lprime[0]);
    prime_position = simd_set(gn, gn);
    prime_next = simd_set(tg, tg);
    parameter = simd_set(m[1], m[0]);

#if defined(LONG_SPRNG)
    unsigned long int lmultiplier[SIMD_NUM_STREAMS];
    simd_store(lmultiplier, multiplier); 
    for (i = 0; i < SIMD_NUM_STREAMS; ++i) {
        if (lmultiplier[i] == 0)
            lmultiplier[i] = CONFIG.MULT[m[i]];
    }
    multiplier = simd_set(lmultiplier[1], lmultiplier[0]);

    vs = simd_sll_64(vs, 0x10);
    seed = simd_xor(seed, vs);

    for (i = 0; i < SIMD_NUM_STREAMS; ++i)
        if (simd_test_zero(prime, vmsk_lh64[i]) == 1)
            seed = simd_or(seed, vmsk_lsb1[i]);
#endif

    // Run generator for awhile, same runs for each stream
    for (i = 0; i < (CONFIG.LCG_RUNUP * gn); ++i)
        get_rn_dbl();
 
    return 0;
}


SIMD_INT VLCG::get_rn_int()
{
    seed = multiply(seed, multiplier, prime);
    return simd_srl_64(seed, 0x11);
}


SIMD_DBL VLCG::get_rn_dbl()
{
    unsigned long int lseed[2] __attribute__ ((aligned(SIMD_ALIGN)));
    double seedd[2] __attribute__ ((aligned(SIMD_ALIGN)));
    SIMD_DBL vseedd;
    SIMD_DBL vrng = simd_set(CONFIG.TWO_M48);

    seed = multiply(seed, multiplier, prime);

    // NOTE: casting done with CPU, bad!!
    simd_store(lseed, seed);
    seedd[0] = lseed[0];
    seedd[1] = lseed[1];
    vseedd = simd_load(seedd);

    return simd_mul_pd(vseedd, vrng);
}


SIMD_FLT VLCG::get_rn_flt()
{
    unsigned long int lseed[2] __attribute__ ((aligned(SIMD_ALIGN)));
    float seedd[4] __attribute__ ((aligned(SIMD_ALIGN)));
    SIMD_FLT vseedd;
    SIMD_FLT vrng = simd_set((float)CONFIG.TWO_M48);

    seed = multiply(seed, multiplier, prime);

    // NOTE: casting done with CPU, bad!!
    simd_store(lseed, seed);
    seedd[0] = lseed[0];
    seedd[1] = 0.0;
    seedd[2] = lseed[1];
    seedd[3] = 0.0;
    vseedd = simd_load(seedd);

    return simd_mul_ps(vseedd, vrng);
    //return simd_cvt_pd2ps(get_rn_dbl());
}


SIMD_INT VLCG::get_seed_rng() const { return init_seed; }
unsigned long int VLCG::get_ngens() const { return LCG_NGENS; }


#if defined(DEBUG)
SIMD_INT VLCG::get_seed() const { return seed; }
SIMD_INT VLCG::get_prime() const { return prime; }
SIMD_INT VLCG::get_multiplier() const { return multiplier; }
#endif


#endif // SIMD_MODE


/***********************************************************************************
* SPRNG (c) 2016 by The University of Tennessee, Knoxville                         *
*                                                                                  *
* SPRNG is licensed under a                                                        *
* Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License. *
*                                                                                  *
* You should have received a copy of the license along with this                   *
* work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.           *
************************************************************************************/

