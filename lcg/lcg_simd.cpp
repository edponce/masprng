/*************************************************************************/
/*************************************************************************/
/*             Parallel 48 bit Linear Congruential Generator             */
/*                                                                       */ 
/* Modified by: Eduardo Ponce                                            */
/*              The University of Tennessee, Knoxville                   */
/*              Email: eponcemo@utk.edu (Jan 2017)                       */
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


// Global SIMD_masks
static SIMD_INT vmsk_lsb1[3];
static SIMD_INT vmsk_lh64[3];
static SIMD_INT vmsk_hi64;
static SIMD_INT vmsk_lsb24;
static SIMD_INT vmsk_lsb48;
static SIMD_INT vmsk_lsb31;
static SIMD_INT vmsk_msbset64;
static SIMD_INT vmsk_msbset24;
/*!
 *  Initialize global SIMD masks
 */
static void init_simd_masks()
{
    // Set vector masks
    vmsk_lsb1[0] = simd_set(0x1UL);                       // all 
    vmsk_lsb1[1] = simd_set(0x0UL, 0x1UL);                // first
    vmsk_lsb1[2] = simd_set(0x1UL, 0x0UL);                // second
    vmsk_lh64[0] = simd_set(0xFFFFFFFFFFFFFFFFUL);        // all 
    vmsk_lh64[1] = simd_set(0x0UL, 0xFFFFFFFFFFFFFFFFUL); // first
    vmsk_lh64[2] = simd_set(0xFFFFFFFFFFFFFFFFUL, 0x0UL); // second
    vmsk_hi64 = simd_set(0xFFFFFFFF00000000UL);           // all
    vmsk_lsb24 = simd_set(0xffffff); 
    vmsk_lsb48 = simd_set(0xffffffffffffL); 
    vmsk_lsb31 = simd_set(0x7FFFFFFFUL);                   // only 31 LSB of seed considered
    vmsk_msbset64 = simd_set(0x3ff0000000000000L);
    vmsk_msbset24 = simd_set(0xff0000);
}


//static SIMD_INT load_idx() {}


unsigned long int VLCG::LCG_NGENS = 0;


VLCG::VLCG()
{
    rng_type = SPRNG_LCG;
    simd_set_zero(&init_seed);
    simd_set_zero(&prime);
    simd_set_zero(&prime_position);
    simd_set_zero(&prime_next);
    simd_set_zero(&parameter);
    gentype = CONFIG.GENTYPE; // NOTE: refactor this, use std::string or char[] with strncpy

#if defined(LONG_SPRNG) 
    simd_set_zero(&seed);
    simd_set_zero(&multiplier);
#else
    simd_set_zero(&seed[0]);
    seed[1] = simd_set(1);
    for (int i = 0; i < 4; ++i)
        simd_set_zero(&multiplier[i]);
#endif

    init_simd_masks();

    ++LCG_NGENS;
}


VLCG::~VLCG()
{
    --LCG_NGENS;
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
/*!
 *  LCG multiply 48-bits using 32-bits.
 */
void VLCG::multiply(SIMD_INT * const a, SIMD_INT * const b, const SIMD_INT c) const
{
    SIMD_INT s[4], res[4];
    SIMD_INT vtmp, vtmp2;

    s[0] = simd_set(4095);
    s[0] = simd_and(a[1], s[0]);
    s[1] = simd_srl_32(a[1], 0xc);
    s[2] = simd_set(4095);
    s[2] = simd_and(a[0], s[2]);
    s[3] = simd_srl_32(a[0], 0xc);

    res[0] = simd_mullo_32(b[0], s[0]);
    res[1] = simd_mullo_32(b[1], s[0]);
    vtmp = simd_mullo_32(b[0], s[1]);   
    res[1] = simd_add_32(res[1], vtmp);
    res[2] = simd_mullo_32(b[2], s[0]);
    vtmp = simd_mullo_32(b[1], s[1]);   
    res[2] = simd_add_32(res[2], vtmp);
    vtmp = simd_mullo_32(b[0], s[2]);   
    res[2] = simd_add_32(res[2], vtmp);
    res[3] = simd_mullo_32(b[3], s[0]);
    vtmp = simd_mullo_32(b[2], s[1]);   
    res[3] = simd_add_32(res[3], vtmp);
    vtmp = simd_mullo_32(b[1], s[2]);   
    res[3] = simd_add_32(res[3], vtmp);
    vtmp = simd_mullo_32(b[0], s[3]);   
    res[3] = simd_add_32(res[3], vtmp);
 
    vtmp = simd_srl_32(a[1], 0x18); 
    vtmp = simd_add_32(vtmp, res[2]);
    vtmp2 = simd_srl_32(res[1], 0xc);
    vtmp = simd_add_32(vtmp, vtmp2);
    vtmp2 = simd_sll_32(res[3], 0xc);
    a[0] = simd_add_32(vtmp, vtmp2);

    vtmp = simd_set(4095);
    vtmp = simd_and(res[1], vtmp);
    vtmp = simd_sll_32(vtmp, 0xc);
    vtmp = simd_add_32(res[0], vtmp);
    a[1] = simd_add_32(vtmp, c);

    vtmp = simd_set(16777215);
    a[0] = simd_and(a[0], vtmp);
    a[1] = simd_and(a[1], vtmp);
}
#endif


/*!
 *  Initialize SIMD RNG
 *  The gn parameter is the same for all streams since the generators are run
 *  for a while based on gn. This also applies to tg parameter.
 */
int VLCG::init_rng(int gn, int tg, int * const s, int * const m)
{
    int i;
    const int need = 1;
    int lprime;
    SIMD_INT vs;

    if (tg <= 0) {
        printf("ERROR: total_gen out of range, %d\n", tg);
        tg = 1;
    }

    if (gn < 0 || gn >= tg || gn >= CONFIG.LCG_MAX_STREAMS) {
        printf("ERROR: gennum out of range, %d\n", gn);
        return -1;
    }
    getprime_32(need, &lprime, gn);

    for (i = 0; i < SIMD_NUM_STREAMS; ++i) {
        if (m[i] < 0 || m[i] >= CONFIG.NPARAMS) {
            printf("ERROR: multiplier out of range, %d\n", m[i]);
            m[i] = 0;
        }
    }

#if defined(LONG_SPRNG)
    //parameter = simd_set(m[1], m[0]);
    parameter = simd_set(m, SIMD_NUM_STREAMS);
    //vs = simd_set(s[1], s[0]);
    vs = simd_set(s, SIMD_NUM_STREAMS);
    init_seed = simd_and(vs, vmsk_lsb31);

    prime_next = simd_set_64(tg);
    prime_position = simd_set_64(gn);

    multiplier = simd_set(CONFIG.MULT[m[1]], CONFIG.MULT[m[0]]);

    vs = simd_sll_64(init_seed, 0x10);
    seed = simd_set(CONFIG.INIT_SEED);
    seed = simd_xor(seed, vs);

    prime = simd_set_64(lprime);
    for (i = 0; i < SIMD_NUM_STREAMS; ++i)
        if (simd_test_zero(prime, vmsk_lh64[i]) == 1)
            seed = simd_or(seed, vmsk_lsb1[i]);
#else
    parameter = simd_set(m[3], m[2], m[1], m[0]);
    vs = simd_set(s[3], s[2], s[1], s[0]);
    init_seed = simd_and(vs, vmsk_lsb31);

    prime_next = simd_set(tg);
    prime_position = simd_set(gn);

    for (i = 0; i < SIMD_NUM_STREAMS; ++i)
        multiplier[i] = simd_load(CONFIG.MULT[m[i]]);

    SIMD_INT vtmp;
    seed[0] = simd_set(CONFIG.INIT_SEED[0]);
    vtmp = simd_srl_32(init_seed, 0x8); 
    vtmp = simd_and(vtmp, vmsk_lsb24);
    seed[0] = simd_or(seed[0], vtmp);

    simd_print("seed[0]", seed[0]);

    seed[1] = simd_set(CONFIG.INIT_SEED[1]);
    vtmp = simd_sll_32(init_seed, 0x10); 
    vtmp = simd_and(vtmp, vmsk_msbset24);
    seed[1] = simd_or(seed[1], vtmp);

    simd_print("seed[1]", seed[1]);

    prime = simd_set(lprime);
    // NOTE: prime check/set is incomplete
#endif

    // Run generator for a while, same runs for each stream
    for (i = 0; i < (CONFIG.LCG_RUNUP * gn); ++i)
        get_rn_dbl();
 
    return 0;
}


SIMD_INT VLCG::get_rn_int()
{
#if defined(LONG_SPRNG)
    seed = multiply(seed, multiplier, prime);
    return simd_srl_64(seed, 0x11);
#else
    multiply(seed, multiplier, prime);
    SIMD_INT vs1 = simd_sll_32(seed[0], 0x7);
    SIMD_INT vs2 = simd_srl_32(seed[1], 0x11);
    return simd_or(vs1, vs2);
#endif
}


SIMD_DBL VLCG::get_rn_dbl()
{
#if defined(LONG_SPRNG)
    const SIMD_DBL vfac = simd_set(CONFIG.TWO_M48);
    SIMD_DBL vseedd;
    unsigned long int lseed[2] __attribute__ ((aligned(SIMD_ALIGN)));
    double seedd[2] __attribute__ ((aligned(SIMD_ALIGN)));

    // NOTE: casting done with CPU, bad!!
    seed = multiply(seed, multiplier, prime);
    simd_store(lseed, seed);
    seedd[0] = lseed[0];
    seedd[1] = lseed[1];
    vseedd = simd_load(seedd);

    return simd_mul_pd(vseedd, vfac);
#else
    const SIMD_DBL vfac[2] = {simd_set(CONFIG.TWO_M24), simd_set(CONFIG.TWO_M48)};
    SIMD_DBL vseedd[2];
    int lseed[4] __attribute__ ((aligned(SIMD_ALIGN)));
    double seedd[2] __attribute__ ((aligned(SIMD_ALIGN)));

    // NOTE: casting done with CPU, bad!!
    multiply(seed, multiplier, prime);
    simd_store(lseed, seed[0]);
    seedd[0] = lseed[0];
    seedd[1] = lseed[2];
    vseedd[0] = simd_load(seedd);

    simd_store(lseed, seed[1]);
    seedd[0] = lseed[0];
    seedd[1] = lseed[2];
    vseedd[1] = simd_load(seedd);

    SIMD_DBL vs1 = simd_mul_pd(vseedd[0], vfac[0]);
    SIMD_DBL vs2 = simd_mul_pd(vseedd[1], vfac[1]);
 
    return simd_add_pd(vs1, vs2);
#endif
}


SIMD_FLT VLCG::get_rn_flt()
{
    SIMD_FLT vrng = simd_set((float)CONFIG.TWO_M48);

#if defined(LONG_SPRNG)
    SIMD_FLT vseedd;
    unsigned long int lseed[2] __attribute__ ((aligned(SIMD_ALIGN)));
    float seedd[4] __attribute__ ((aligned(SIMD_ALIGN)));

    // NOTE: casting done with CPU, bad!!
    seed = multiply(seed, multiplier, prime);
    simd_store(lseed, seed);
    seedd[0] = lseed[0];
    //seedd[1] = lseed[1];

    //seed = multiply(seed, multiplier, prime);
    //simd_store(lseed, seed);
    seedd[2] = lseed[0];
    //seedd[3] = lseed[1];
    vseedd = simd_load(seedd);

    return simd_mul_ps(vseedd, vrng);
    //return simd_cvt_pd2ps(get_rn_dbl());
#else
    multiply(seed, multiplier, prime);
    return simd_mul_ps(vrng, vrng);
#endif
}


SIMD_INT VLCG::get_seed_rng() const { return init_seed; }
unsigned long int VLCG::get_ngens() const { return LCG_NGENS; }


#if defined(DEBUG)
SIMD_INT VLCG::get_prime() const { return prime; }
#if defined(LONG_SPRNG)
SIMD_INT VLCG::get_seed() const { return seed; }
SIMD_INT VLCG::get_multiplier() const { return multiplier; }
#else
SIMD_INT VLCG::get_seed() const { return seed[0]; }
SIMD_INT VLCG::get_multiplier() const { return multiplier[0]; }
#endif
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

