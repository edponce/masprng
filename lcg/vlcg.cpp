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


#include "simd.h"
#if defined(SIMD_MODE)


#include <cstdio>
#include <climits>
#include "vlcg.h"
#include "lcg_globals.h"
#include "primes_32.h"


unsigned long int VLCG::LCG_NGENS = 0;


VLCG::VLCG()
{
    rng_type = VSPRNG_LCG;
    simd_set_zero(&init_seed);
    simd_set_zero(&prime);
    simd_set_zero(&prime_position);
    simd_set_zero(&prime_next);
    simd_set_zero(&parameter);
    gentype = GLOBALS.GENTYPE;

#if defined(LONG_SPRNG) 
    simd_set_zero(&seed);
    simd_set_zero(&multiplier);
#else
    simd_set_zero(&seed[0]);
    seed[1] = simd_set((int)0x1);
    for (int i = 0; i < 4; ++i)
        simd_set_zero(&multiplier[i]);
#endif

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
    const SIMD_INT vmsk_lsb48 = simd_set(0xFFFFFFFFFFFFUL); 
    SIMD_INT res = simd_mul_u64(a, b);
    res = simd_add_i64(res, c);
    return simd_and(res, vmsk_lsb48);
}
#else
/*!
 *  LCG multiply 48-bits using 32-bits.
 */
void VLCG::multiply(SIMD_INT * const a, SIMD_INT * const b, const SIMD_INT c) const
{
    SIMD_INT res[4], vtmp[3];
    const SIMD_INT vmsk_4095 = simd_set(4095U);
    const SIMD_INT vmsk_167_ = simd_set(16777215U);
    const SIMD_INT s[4] = { simd_and(a[1], vmsk_4095),
                               simd_srl_32(a[1], 0xc),
                            simd_and(a[0], vmsk_4095),
                               simd_srl_32(a[0], 0xc) };

    for (int i = 0; i < 4; ++i) {
        res[i] = simd_mullo_i32(b[0], s[i]);
        for (int j = 0; j < i; ++j) {
            const SIMD_INT vmul = simd_mullo_i32(b[i-j], s[j]);
            res[i] = simd_add_i32(res[i], vmul);
        }
    }

    vtmp[0] = simd_srl_32(a[1], 0x18); 
    vtmp[1] = simd_srl_32(res[1], 0xc);
    vtmp[2] = simd_sll_32(res[3], 0xc);
    vtmp[0] = simd_add_i32(vtmp[0], res[2]);
    vtmp[1] = simd_add_i32(vtmp[1], vtmp[2]);
    a[0] = simd_add_i32(vtmp[0], vtmp[1]);
    a[0] = simd_and(a[0], vmsk_167_);

    vtmp[0] = simd_and(res[1], vmsk_4095);
    vtmp[0] = simd_sll_32(vtmp[0], 0xc);
    vtmp[1] = simd_add_i32(res[0], c);
    a[1] = simd_add_i32(vtmp[0], vtmp[1]);
    a[1] = simd_and(a[1], vmsk_167_);
}
#endif


/*!
 *  Initialize SIMD RNG
 *  NOTE: The gn parameter is the same for all streams since the generators are run
 *  for a while based on gn. This also applies to tg parameter.
 */
int VLCG::init_rng(int gn, int tg, int * const s, int * const m)
{
    if (tg <= 0) {
        printf("ERROR: total_gen out of range, %d\n", tg);
        tg = 1;
    }

    if (gn >= GLOBALS.LCG_MAX_STREAMS)
        printf("WARNING: generator number (%d) is greater than maximum number of independent streams (%d), independence of streams cannot be guaranteed.\n", gn, GLOBALS.LCG_MAX_STREAMS);

    if (gn < 0 || gn >= tg) {
        printf("ERROR: generator number is out of range, %d\n", gn);
        return -1;
    }
    const int need = 1;
    int lprime;
    getprime_32(need, &lprime, gn);

#if defined(LONG_SPRNG)
    for (int i = 0; i < SIMD_STREAMS_64; ++i) {
        if (m[i] < 0 || m[i] >= GLOBALS.NPARAMS) {
            printf("ERROR: multiplier out of range, %d\n", m[i]);
            m[i] = 0;
        }
    }
    parameter = simd_set(m, SIMD_STREAMS_64);

    const SIMD_INT vmsk_lsb31 = simd_set(0x7FFFFFFFUL);
    SIMD_INT vs = simd_set(s, SIMD_STREAMS_64);
    init_seed = simd_and(vs, vmsk_lsb31);

    prime_next = simd_set_64(tg);
    prime_position = simd_set_64(gn);

    long int lmultiplier[SIMD_STREAMS_64];
    for (int i = 0; i < SIMD_STREAMS_64; ++i)
        lmultiplier[i] = GLOBALS.MULT[m[i]];
    multiplier = simd_set(lmultiplier, SIMD_STREAMS_64);

    vs = simd_sll_64(init_seed, 0x10);
    seed = simd_set(GLOBALS.INIT_SEED);
    seed = simd_xor(seed, vs);

    const SIMD_INT vmsk_lsb1 = simd_set(0x1UL, 0x1UL);
    prime = simd_set_64(lprime);
    if (lprime == 0)
        seed = simd_or(seed, vmsk_lsb1);
#else
    for (int i = 0; i < SIMD_STREAMS_32; ++i) {
        if (m[i] < 0 || m[i] >= GLOBALS.NPARAMS) {
            printf("ERROR: multiplier out of range, %d\n", m[i]);
            m[i] = 0;
        }
    }
    parameter = simd_set(m, SIMD_STREAMS_32);

    const SIMD_INT vmsk_lsb31 = simd_set(0x7FFFFFFFU);
    SIMD_INT vs = simd_set(s, SIMD_STREAMS_32);
    init_seed = simd_and(vs, vmsk_lsb31);

    prime_next = simd_set(tg);
    prime_position = simd_set(gn);

    int lmultiplier[SIMD_STREAMS_32];
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < SIMD_STREAMS_32; ++i) {
            lmultiplier[i] = GLOBALS.MULT[m[i]][j]; 
        }
        multiplier[j] = simd_set(lmultiplier, SIMD_STREAMS_32);
    }

    const SIMD_INT vmsk_lsb24 = simd_set(0xFFFFFFU); 
    vs = simd_srl_32(init_seed, 0x8); 
    vs = simd_and(vs, vmsk_lsb24);
    seed[0] = simd_set(GLOBALS.INIT_SEED[0]);
    seed[0] = simd_xor(seed[0], vs);

    const SIMD_INT vmsk_msb8_24 = simd_set(0xFF0000U);
    vs = simd_sll_32(init_seed, 0x10); 
    vs = simd_and(vs, vmsk_msb8_24);
    seed[1] = simd_set(GLOBALS.INIT_SEED[1]);
    seed[1] = simd_xor(seed[1], vs);

    const SIMD_INT vmsk_lsb1 = simd_set(0x1U);
    prime = simd_set(lprime);
    if (lprime == 0)
        seed[1] = simd_or(seed[1], vmsk_lsb1);
#endif

    // Run generator for a while, same runs for each stream
    for (int i = 0; i < (GLOBALS.LCG_RUNUP * gn); ++i)
        get_rn_dbl();
 
    return 0;
}


// NOTE: should return contiguous ints?
SIMD_INT VLCG::get_rn_int()
{
#if defined(LONG_SPRNG)
    seed = multiply(seed, multiplier, prime);
    return simd_srl_64(seed, 0x11);
#else
    multiply(seed, multiplier, prime);
    const SIMD_INT vs[2] = { simd_sll_32(seed[0], 0x7),
                            simd_srl_32(seed[1], 0x11) };
    return simd_or(vs[0], vs[1]);
#endif
}


SIMD_DBL VLCG::get_rn_dbl()
{
#if defined(LONG_SPRNG)
    const SIMD_DBL vfac = simd_set(GLOBALS.TWO_M48);
    seed = multiply(seed, multiplier, prime);
    const SIMD_DBL seed_dbl = simd_cvt_u64_f64(seed);
    return simd_mul(seed_dbl, vfac);
#else
    const SIMD_DBL vfac[2] = { simd_set(GLOBALS.TWO_M24),
                               simd_set(GLOBALS.TWO_M48) };
    multiply(seed, multiplier, prime);
    const SIMD_DBL seed_dbl[2] = { simd_cvt_i32_f64(seed[0]),
                                   simd_cvt_i32_f64(seed[1]) };
    const SIMD_DBL vs[2] = { simd_mul(seed_dbl[0], vfac[0]),
                             simd_mul(seed_dbl[1], vfac[1]) };
    return simd_add(vs[0], vs[1]);
#endif
}


SIMD_FLT VLCG::get_rn_flt()
{
#if defined(LONG_SPRNG)
    const SIMD_FLT vfac = simd_set((float)GLOBALS.TWO_M48);
    seed = multiply(seed, multiplier, prime);
    const SIMD_FLT seed_flt = simd_cvt_u64_f32(seed);
    return simd_mul(seed_flt, vfac);
#else
    const SIMD_FLT vfac[2] = { simd_set((float)GLOBALS.TWO_M24),
                               simd_set((float)GLOBALS.TWO_M48) };
    multiply(seed, multiplier, prime);
    const SIMD_FLT seed_flt[2] = { simd_cvt_i32_f32(seed[0]),
                                   simd_cvt_i32_f32(seed[1]) };
    const SIMD_FLT vs[2] = { simd_mul(seed_flt[0], vfac[0]),
                             simd_mul(seed_flt[1], vfac[1]) };
    return simd_add(vs[0], vs[1]);
#endif
}


SIMD_INT VLCG::get_seed_rng() const { return init_seed; }
unsigned long int VLCG::get_ngens() const { return LCG_NGENS; }


#if defined(DEBUG)
SIMD_INT VLCG::get_prime() const { return prime; }
# if defined(LONG_SPRNG)
SIMD_INT VLCG::get_seed() const { return seed; }
SIMD_INT VLCG::get_multiplier() const { return multiplier; }
# else
SIMD_INT VLCG::get_seed() const { return seed[0]; }
SIMD_INT VLCG::get_multiplier() const { return multiplier[0]; }
# endif
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

