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
    prime_position = 0;
    prime_next = 0;
    gentype = GLOBALS.GENTYPE;
#if defined(LONG_SPRNG) 
    for (int i = 0; i < 2; ++i) {
        simd_set_zero(&init_seed[i]);
        simd_set_zero(&parameter[i]);
        simd_set_zero(&prime[i]);
        simd_set_zero(&seed[i]);
        simd_set_zero(&multiplier[i]);
    }
#else
    simd_set_zero(&init_seed);
    simd_set_zero(&parameter);
    simd_set_zero(&prime);
    simd_set_zero(&seed[0]);
    seed[1] = simd_set(0x1U);
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
    const SIMD_INT vmsk_4095 = simd_set(4095U);
    const SIMD_INT vmsk_167_ = simd_set(16777215U);
    SIMD_INT s[4], res[4], vtmp[3];

    s[0] = simd_and(a[1], vmsk_4095);
    s[1] = simd_srl_32(a[1], 0xc);
    s[2] = simd_and(a[0], vmsk_4095);
    s[3] = simd_srl_32(a[0], 0xc);

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
    prime_next = tg;

    if (gn >= GLOBALS.LCG_MAX_STREAMS)
        printf("WARNING: generator number (%d) is greater than maximum number of independent streams (%d), independence of streams cannot be guaranteed.\n", gn, GLOBALS.LCG_MAX_STREAMS);

    if (gn < 0 || gn >= tg) {
        printf("ERROR: generator number is out of range, %d\n", gn);
        return -1;
    }

    int lprime;
    prime_position = gn;
    getprime_32(1, &lprime, prime_position);

#if defined(LONG_SPRNG)
    for (int i = 0; i < SIMD_STREAMS_32; ++i) {
        if (m[i] < 0 || m[i] >= GLOBALS.NPARAMS) {
            printf("ERROR: multiplier out of range, %d\n", m[i]);
            m[i] = 0;
        }
    }

for (int j = 0; j < SIMD_STREAMS_64; ++j) {
    const int k = j * SIMD_STREAMS_64;

    parameter[j] = simd_set(&m[k], SIMD_STREAMS_64);

    const SIMD_INT vmsk_lsb31 = simd_set(0x7FFFFFFFUL);
    SIMD_INT vs = simd_set(&s[k], SIMD_STREAMS_64);
    init_seed[j] = simd_and(vs, vmsk_lsb31);

    long int lmultiplier[SIMD_STREAMS_64];
    for (int i = 0; i < SIMD_STREAMS_64; ++i)
        lmultiplier[i] = GLOBALS.MULT[m[i+k]];
    multiplier[j] = simd_set(lmultiplier, SIMD_STREAMS_64);

    vs = simd_sll_64(init_seed[j], 0x10);
    seed[j] = simd_set(GLOBALS.INIT_SEED);
    seed[j] = simd_xor(seed[j], vs);

    const SIMD_INT vmsk_lsb1 = simd_set(0x1UL);
    prime[j] = simd_set_64(lprime);
    if (lprime == 0)
        seed[j] = simd_or(seed[j], vmsk_lsb1);
}
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

    // Run generator several times
    for (int i = 0; i < (GLOBALS.LCG_RUNUP * prime_position); ++i)
        get_rn_dbl();
 
    return 0;
}


SIMD_INT VLCG::get_rn_int()
{
#if defined(LONG_SPRNG)
    const int shfl_ctrl[2] = { 0x58, 0x85 };
    SIMD_INT rn[2];

    for (int i = 0; i < SIMD_STREAMS_64; ++i) {
        seed[i] = multiply(seed[i], multiplier[i], prime[i]);
        rn[i] = simd_srl_64(seed[i], 0x11);
        rn[i] = simd_shuffle_i32(rn[i], shfl_ctrl[i]);
    }

    return simd_or(rn[0], rn[1]);
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

    seed[0] = multiply(seed[0], multiplier[0], prime[0]);
    const SIMD_DBL seed_dbl = simd_cvt_u64_f64(seed[0]);

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
    SIMD_FLT rn[2];

    for (int i = 0; i < SIMD_STREAMS_64; ++i) {
        seed[i] = multiply(seed[i], multiplier[i], prime[i]);
        const SIMD_FLT seed_flt = simd_cvt_u64_f32(seed[i]);
        rn[i] = simd_mul(seed_flt, vfac);
    }

    return simd_shuffle_f32(rn[0], rn[1], 0x88U);
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


#if defined(LONG_SPRNG)
SIMD_INT VLCG::get_seed_rng() const { return init_seed[0]; }
#else
SIMD_INT VLCG::get_seed_rng() const { return init_seed; }
#endif
unsigned long int VLCG::get_ngens() const { return LCG_NGENS; }


#if defined(DEBUG)
SIMD_INT VLCG::get_seed() const { return seed[0]; }
SIMD_INT VLCG::get_multiplier() const { return multiplier[0]; }
# if defined(LONG_SPRNG)
SIMD_INT VLCG::get_prime() const { return prime[0]; }
# else
SIMD_INT VLCG::get_prime() const { return prime; }
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

