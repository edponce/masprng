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


#include <stdio.h>   // printf
#include <string.h>  // memset
#include "vlcg.h"
#include "lcg_globals.h"
#include "primes_32.h"
#include "vutils.h"


int VLCG::LCG_NGENS = 0;


/*!
 *  \brief Constructor (no parameters)
 */
VLCG::VLCG()
{
    gentype = GLOBALS.GENTYPE;
    rng_type = VSPRNG_LCG;
    prime_position = 0;
    prime_next = 0;
#if defined(LONG_SPRNG) 
	simd_malloc(&init_seed, SIMD_WIDTH_BYTES, 2);
    simd_set_zero(&init_seed[0]);
    simd_set_zero(&init_seed[1]);

	simd_malloc(&parameter, SIMD_WIDTH_BYTES, 2);
    simd_set_zero(&parameter[0]);
    simd_set_zero(&parameter[1]);

	simd_malloc(&prime, SIMD_WIDTH_BYTES, 2);
    simd_set_zero(&prime[0]);
    simd_set_zero(&prime[1]);

	simd_malloc(&seed, SIMD_WIDTH_BYTES, 2);
    simd_set_zero(&seed[0]);
    simd_set_zero(&seed[1]);

	simd_malloc(&multiplier, SIMD_WIDTH_BYTES, 2);
    simd_set_zero(&multiplier[0]);
    simd_set_zero(&multiplier[1]);
#else
	simd_malloc(&init_seed, SIMD_WIDTH_BYTES, 1);
    simd_set_zero(&init_seed[0]);

	simd_malloc(&parameter, SIMD_WIDTH_BYTES, 1);
    simd_set_zero(&parameter[0]);

	simd_malloc(&prime, SIMD_WIDTH_BYTES, 1);
    simd_set_zero(&prime[0]);

	simd_malloc(&seed, SIMD_WIDTH_BYTES, 2);
    simd_set_zero(&seed[0]);
    seed[1] = simd_set(0x1U);

	simd_malloc(&multiplier, SIMD_WIDTH_BYTES, 4);
    for (int32_t i = 0; i < 4; ++i)
        simd_set_zero(multiplier+i);
#endif

    ++LCG_NGENS;
}


/*!
 *  \brief Destructor
 */
VLCG::~VLCG()
{
	free(init_seed);
	free(parameter);
	free(prime);
	free(seed);
	free(multiplier);

    --LCG_NGENS;
}


#if defined(LONG_SPRNG)
/*!
 *  \brief LCG multiply 48-bits using 64-bits.
 */
void VLCG::multiply(SIMD_INT * const a, const SIMD_INT * const b, const SIMD_INT * const c) const
{
    const SIMD_INT vmsk_lsb48 = simd_set(0xFFFFFFFFFFFFUL); 

    a[0] = simd_mul_u64(a[0], b[0]);
    a[0] = simd_add_i64(a[0], c[0]);
    a[0] = simd_and(a[0], vmsk_lsb48);
}
#else
/*!
 *  \brief LCG multiply 48-bits using 32-bits.
 */
// NOTE: const function?
void VLCG::multiply(SIMD_INT * const a, const SIMD_INT * const b, const SIMD_INT * const c) const
{
    
    const SIMD_INT vmsk_fac[2] __SIMD_SET_ALIGNED = { simd_set(4095U),
                                                  simd_set(16777215U) };
    SIMD_INT s[4] __SIMD_SET_ALIGNED;
    SIMD_INT res[4] __SIMD_SET_ALIGNED;
    SIMD_INT vtmp[3] __SIMD_SET_ALIGNED;

    s[0] = simd_and(a[1], vmsk_fac[0]);
    s[2] = simd_and(a[0], vmsk_fac[0]);
    s[3] = simd_srl_32(a[0], 0xc);
    s[1] = simd_srl_32(a[1], 0xc);

    // NOTE: check info on vectorization compiler hints
    #pragma vector aligned
    for (int32_t i = 0; i < 4; ++i) {
        SIMD_INT * const res_ptr __SIMD_SET_ALIGNED = res + i;

        *res_ptr = simd_mullo_i32(b[0], *(s+i));
        for (int32_t j = 0; j < i; ++j) {
            const SIMD_INT vmul = simd_mullo_i32(*(b+i-j), *(s+j));
            *res_ptr = simd_add_i32(*res_ptr, vmul);
        }
    }

    vtmp[0] = simd_srl_32(a[1], 0x18); 
    vtmp[1] = simd_srl_32(res[1], 0xc);
    vtmp[2] = simd_sll_32(res[3], 0xc);
    vtmp[0] = simd_add_i32(vtmp[0], res[2]);
    vtmp[1] = simd_add_i32(vtmp[1], vtmp[2]);
    vtmp[0] = simd_add_i32(vtmp[0], vtmp[1]);
    a[0] = simd_and(vtmp[0], vmsk_fac[1]);

    vtmp[0] = simd_and(res[1], vmsk_fac[0]);
    vtmp[1] = simd_add_i32(res[0], c[0]);
    vtmp[0] = simd_sll_32(vtmp[0], 0xc);
    vtmp[0] = simd_add_i32(vtmp[0], vtmp[1]);
    a[1] = simd_and(vtmp[0], vmsk_fac[1]);
}
#endif


/*!
 *  \brief Initialize RNG
 *
 *  NOTE: The gn parameter is the same for all streams since the generators are run
 *  for a while based on gn. This also applies to tg parameter.
 *  Consequently, a single and same prime number is used for all streams.
 *
 *  NOTE: int and float streams use all parameters from input arrays,
 *  double stream use the first half of the parameters from input arrays.
 *
 *  NOTE: need to validate s and m input arrays for sufficient values.
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
    int32_t lprime;
    prime_position = gn;
    getprime_32(1, &lprime, prime_position);

    if (!m) {
        const int32_t m_default = 0;
        printf("WARNING: no array for multipliers provided, default is %d\n", m_default);
        memset(m, m_default, SIMD_STREAMS_32 * sizeof(int32_t));
    }
    else {
        for (int32_t strm = 0; strm < SIMD_STREAMS_32; ++strm) {
            if (m[strm] < 0 || m[strm] >= GLOBALS.NPARAMS) {
                printf("ERROR: multiplier out of range, %d\n", m[strm]);
                m[strm] = 0;
            }
        }
    }

    if (!s) {
        const int s_default = 0;
        printf("WARNING: no array for seeds provided, default is %d\n", s_default);
        memset(s, s_default, SIMD_STREAMS_32 * sizeof(int32_t));
    }

#if defined(LONG_SPRNG)
    parameter[0] = simd_set(&m[0], SIMD_STREAMS_64);
    parameter[1] = simd_set(&m[SIMD_STREAMS_64], SIMD_STREAMS_64);

    int64_t lmultiplier[2][SIMD_STREAMS_64] __SIMD_SET_ALIGNED;
    for (int32_t i = 0; i < SIMD_STREAMS_64; ++i) {
        lmultiplier[0][i] = GLOBALS.MULT[m[i]];
        lmultiplier[1][i] = GLOBALS.MULT[m[i+SIMD_STREAMS_64]];
    }
    multiplier[0] = simd_set(lmultiplier[0], SIMD_STREAMS_64);
    multiplier[1] = simd_set(lmultiplier[1], SIMD_STREAMS_64);

    SIMD_INT vs[2] __SIMD_SET_ALIGNED;
    vs[0] = simd_set(&s[0], SIMD_STREAMS_64);
    vs[1] = simd_set(&s[SIMD_STREAMS_64], SIMD_STREAMS_64);

    const SIMD_INT vmsk_lsb31 = simd_set(0x7FFFFFFFUL);
    init_seed[0] = simd_and(vs[0], vmsk_lsb31);
    init_seed[1] = simd_and(vs[1], vmsk_lsb31);

    vs[0] = simd_sll_64(init_seed[0], 0x10);
    vs[1] = simd_sll_64(init_seed[1], 0x10);

    const SIMD_INT vmsk_seed = simd_set(GLOBALS.INIT_SEED);
    seed[0] = simd_xor(vmsk_seed, vs[0]);
    seed[1] = simd_xor(vmsk_seed, vs[1]);

    const SIMD_INT vmsk_lsb1 = simd_set(0x1UL);
    prime[0] = simd_set_64(lprime);
    prime[1] = simd_set_64(lprime);
    if (lprime == 0) {
        seed[0] = simd_or(seed[0], vmsk_lsb1);
        seed[1] = simd_or(seed[1], vmsk_lsb1);
    }

#else
    parameter[0] = simd_set(m, SIMD_STREAMS_32);

    int32_t lmultiplier[SIMD_STREAMS_32] __SIMD_SET_ALIGNED;
    for (int32_t j = 0; j < 4; ++j) {
        for (int32_t i = 0; i < SIMD_STREAMS_32; ++i) {
            lmultiplier[i] = GLOBALS.MULT[m[i]][j]; 
        }
        multiplier[j] = simd_set(lmultiplier, SIMD_STREAMS_32);
    }

    const SIMD_INT vmsk_lsb31 = simd_set(0x7FFFFFFFU);
    const SIMD_INT vs = simd_set(s, SIMD_STREAMS_32);
    init_seed[0] = simd_and(vs, vmsk_lsb31);

    const SIMD_INT vmsk_lsb24 = simd_set(0xFFFFFFU); 
    SIMD_INT vtmp[2] __SIMD_SET_ALIGNED;
    vtmp[0] = simd_srl_32(init_seed[0], 0x8); 
    vtmp[0] = simd_and(vtmp[0], vmsk_lsb24);

    const SIMD_INT vmsk_msb8_24 = simd_set(0xFF0000U);
    vtmp[1] = simd_sll_32(init_seed[0], 0x10); 
    vtmp[1] = simd_and(vtmp[1], vmsk_msb8_24);

    const SIMD_INT vmsk_seed[2] __SIMD_SET_ALIGNED = { simd_set(GLOBALS.INIT_SEED[0]),
                                                       simd_set(GLOBALS.INIT_SEED[1]) };
    seed[0] = simd_xor(vmsk_seed[0], vtmp[0]);
    seed[1] = simd_xor(vmsk_seed[1], vtmp[1]);

    const SIMD_INT vmsk_lsb1 = simd_set(0x1U);
    prime[0] = simd_set(lprime);
    if (lprime == 0)
        seed[1] = simd_or(seed[1], vmsk_lsb1);
#endif

    // Run generator several times
    for (int32_t i = 0; i < (GLOBALS.LCG_RUNUP * prime_position); ++i)
        get_rn_dbl();
 
    return 0;
}


SIMD_INT VLCG::get_rn_int()
{
#if defined(LONG_SPRNG)
    SIMD_INT rn[2] __SIMD_SET_ALIGNED;

    multiply(&seed[0], &multiplier[0], &prime[0]);
    multiply(&seed[1], &multiplier[1], &prime[1]);

    rn[0] = simd_srl_64(seed[0], 0x11);
    rn[1] = simd_srl_64(seed[1], 0x11);

    return simd_packmerge_i32(rn[0], rn[1]);
#else
    SIMD_INT rn[2] __SIMD_SET_ALIGNED;

    multiply(&seed[0], &multiplier[0], &prime[0]);

    rn[0] = simd_sll_32(seed[0], 0x7);
    rn[1] = simd_srl_32(seed[1], 0x11);

    return simd_or(rn[0], rn[1]);
#endif
}


SIMD_DBL VLCG::get_rn_dbl()
{
#if defined(LONG_SPRNG)
    const SIMD_DBL vfac = simd_set(GLOBALS.TWO_M48);
    SIMD_DBL seed_dbl;

    multiply(&seed[0], &multiplier[0], &prime[0]);
    seed_dbl = simd_cvt_u64_f64(seed[0]);

    return simd_mul(seed_dbl, vfac);
#else
    const SIMD_DBL vfac[2] __SIMD_SET_ALIGNED = { simd_set(GLOBALS.TWO_M24),
                                                  simd_set(GLOBALS.TWO_M48) };
    SIMD_DBL rn[2] __SIMD_SET_ALIGNED;
    SIMD_DBL seed_dbl[2] __SIMD_SET_ALIGNED;

    multiply(&seed[0], &multiplier[0], &prime[0]);

    seed_dbl[0] = simd_cvt_i32_f64(seed[0]);
    seed_dbl[1] = simd_cvt_i32_f64(seed[1]);

    rn[0] = simd_mul(seed_dbl[0], vfac[0]);
    rn[1] = simd_mul(seed_dbl[1], vfac[1]);

    return simd_add(rn[0], rn[1]);

    // NOTE: can convert into fused multiply-add
    //return simd_fmadd(seed_dbl[1], vfac[1], rn[0]);
#endif
}


SIMD_FLT VLCG::get_rn_flt()
{
#if defined(LONG_SPRNG)
    const SIMD_FLT vfac = simd_set((float)GLOBALS.TWO_M48);
    SIMD_FLT rn[2] __SIMD_SET_ALIGNED;
    SIMD_FLT seed_flt[2] __SIMD_SET_ALIGNED; 

    multiply(&seed[0], &multiplier[0], &prime[0]);
    multiply(&seed[1], &multiplier[1], &prime[1]);

    seed_flt[0] = simd_cvt_u64_f32(seed[0]);
    seed_flt[1] = simd_cvt_u64_f32(seed[1]);

    rn[0] = simd_mul(seed_flt[0], vfac);
    rn[1] = simd_mul(seed_flt[1], vfac);

    return simd_merge_lo(rn[0], rn[1]);
#else
    const SIMD_FLT vfac[2] __SIMD_SET_ALIGNED = { simd_set((float)GLOBALS.TWO_M24),
                                                  simd_set((float)GLOBALS.TWO_M48) };
    SIMD_FLT rn[2] __SIMD_SET_ALIGNED;
    SIMD_FLT seed_flt[2] __SIMD_SET_ALIGNED; 

    multiply(&seed[0], &multiplier[0], &prime[0]);

    seed_flt[0] = simd_cvt_i32_f32(seed[0]);
    seed_flt[1] = simd_cvt_i32_f32(seed[1]);

    rn[0] = simd_mul(seed_flt[0], vfac[0]);
    rn[1] = simd_mul(seed_flt[1], vfac[1]);

    return simd_add(rn[0], rn[1]);

    // NOTE: can convert into fused multiply-add
    //return simd_fmadd(seed_flt[1], vfac[1], rn[0]);
#endif
}


#if defined(LONG_SPRNG)
SIMD_INT VLCG::get_seed_rng() const { return init_seed[0]; }
#else
SIMD_INT VLCG::get_seed_rng() const { return init_seed[0]; }
#endif
int VLCG::get_ngens() const { return LCG_NGENS; }
#if defined(DEBUG)
SIMD_INT VLCG::get_seed() const { return seed[0]; }
SIMD_INT VLCG::get_multiplier() const { return multiplier[0]; }
# if defined(LONG_SPRNG)
SIMD_INT VLCG::get_prime() const { return prime[0]; }
# else
SIMD_INT VLCG::get_prime() const { return prime[0]; }
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

