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
    gentype = CONFIG.GENTYPE; // NOTE: refactor this

#if defined(LONG_SPRNG) 
    seed = simd_set(CONFIG.INIT_SEED, CONFIG.INIT_SEED);
    for (int i = 0; i < CONFIG.NPARAMS; ++i)
        mults_g[i] = CONFIG.MULT[i];
    multiplier_g = 0;
#else
    seed = simd_set(CONFIG.INIT_SEED[1], CONFIG.INIT_SEED[0]);
    for (int i = 0; i < CONFIG.NPARAMS; ++i)
        for (int j = 0; j < 4; ++j)
            mults_g[i][j] = CONFIG.MULT[i][j];
    multiplier_g = NULL;
#endif

    init_simd_masks();

    //LCG_NGENS += SIMD_NUM_STREAMS;
    ++LCG_NGENS;
}


VLCG::~VLCG()
{
    //LCG_NGENS -= SIMD_NUM_STREAMS;
    --LCG_NGENS;
}


/*!
 *  Initialize global SIMD masks
 */
void VLCG::init_simd_masks()
{
    // Set vector masks
    vmsk_lsb1[0] = simd_set(0x1UL);                         // all 
    vmsk_lsb1[1] = simd_set(0x0ULL, 0x1ULL);                // first
    vmsk_lsb1[2] = simd_set(0x1ULL, 0x0ULL);                // second
    vmsk_lh64[0] = simd_set(0xFFFFFFFFFFFFFFFFULL);         // all 
    vmsk_lh64[1] = simd_set(0x0ULL, 0xFFFFFFFFFFFFFFFFULL); // first
    vmsk_lh64[2] = simd_set(0xFFFFFFFFFFFFFFFFULL, 0x0ULL); // second
    vmsk_hi64 = simd_set(0xFFFFFFFF00000000ULL);            // all
    vmsk_lsb48 = simd_set(CONFIG.LSB48);                    // all
    vmsk_seed = simd_set(0x7FFFFFFFULL);                    // only 31 LSB of seed considered
    simd_set_zero(&vmult_shf);
}


#if defined(LONG_SPRNG)
/*!
 *  LCG multiply 48-bits using 64-bits.
 *  Perform 64-bit integer multiplication using 32-bit integers
 *  x64 * y64 = (xl32 * yl32) + (xl32 * yh32 + xh32 * yl32) * 2^32
 */
SIMD_INT VLCG::multiply_48_64(SIMD_INT a, SIMD_INT b) const
{
    SIMD_INT st, vtmp, u, c = seed;

    // NOTE: require at least SSE 4.1 for simd_mullo_epi32()
    //vmult_shf = simd_shuffle_32(a, 0xB1); // shuffle multiplier 
    st = simd_mullo_32(c, vmult_shf);    // xl * yh, xh * yl
    vtmp = simd_sll_64(st, 0x20);         // shift << 32 
    st = simd_add_64(st, vtmp);           // s + t 
    st = simd_and(st, vmsk_hi64);         // (s + t) & 0xFFFFFFFF00000000
    u = simd_mul_u32(c, a);              // xl * yl
    c = simd_add_64(u, st);              // u + s + t 
    c = simd_add_64(c, b);              // seed += prime
    c = simd_and(c, vmsk_lsb48);        // seed &= LSB48

    return c;
}
#endif


// Initialize SIMD RNG
int VLCG::init_rng(int gn, int tg, int *s, int *m)
{
    int i;
    int need = 1;
    int primes[SIMD_NUM_STREAMS];
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
        getprime_32(need, &primes[i], gn);

        if (m[i] < 0 || m[i] >= CONFIG.NPARAMS) {
            printf("ERROR: multiplier out of range, %d\n", m[i]);
            m[i] = 0;
        }
    }

    vs = simd_set(s[1], s[0]);
    vs = simd_and(vs, vmsk_seed);
    init_seed = vs;

    prime = simd_set(primes[1], primes[0]);
    prime_position = simd_set(gn, gn);
    prime_next = simd_set(tg, tg);
    parameter = simd_set(m[1], m[0]);

#if defined(LONG_SPRNG)
    if (simd_test_zero(multiplier, vmsk_lh64[0]) == 1) { // all streams have multiplier = 0
        multiplier = simd_set(mults_g[m[1]], mults_g[m[0]]);

        // set shuffle multiplier used in multiply() since it does not changes
        vmult_shf = simd_shuffle_32(multiplier, 0xB1);
    }

    vs = simd_sll_64(vs, 0x10);
    seed = simd_xor(seed, vs);

    // NOTE: require at least SSE 4.1 for simd_test_zero()
    // i=0, all streams have prime = 0
    // i=1, first stream has prime = 0
    // i=k, k stream has prime = 0
    for (i = 0; i < SIMD_NUM_STREAMS; ++i)
        if (simd_test_zero(prime, vmsk_lh64[i]) == 1)
            seed = simd_or(seed, vmsk_lsb1[i]);
#endif

    // Run generator
    // NOTE: the factor is not gn, but prime_position. How to do with vector?
    for (i = 0; i < (CONFIG.LCG_RUNUP * gn); ++i)
        get_rn_dbl();
 
    return 0;
}


SIMD_INT VLCG::get_rn_int()
{
    seed = multiply_48_64(multiplier, prime);
    return simd_srl_64(seed, 0x11);
}


SIMD_DBL VLCG::get_rn_dbl()
{
    unsigned long int lseed[2] __attribute__ ((aligned(SIMD_ALIGN)));
    double seedd[2] __attribute__ ((aligned(SIMD_ALIGN)));
    SIMD_DBL vseedd;
    SIMD_DBL vrng = simd_set(CONFIG.TWO_M48);

    seed = multiply_48_64(multiplier, prime);

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

    seed = multiply_48_64(multiplier, prime);

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


// NOTE: debug purposes
SIMD_INT VLCG::get_seed() { return seed; }
SIMD_INT VLCG::get_prime() { return prime; }
SIMD_INT VLCG::get_multiplier() { return multiplier; }


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

