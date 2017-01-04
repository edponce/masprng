/*************************************************************************/
/*************************************************************************/
/*             Parallel 48 bit Linear Congruential Generator             */
/*                                                                       */ 
/* Modified by: Eduardo Ponce                                            */
/*              The University of Tennessee, Knoxville                   */
/*              Email: eponcemo@utk.edu (Jan 2017)                       */
/*                                                                       */
/* Modified by: J. Ren                                                   */
/*             Florida State University                                  */
/*             Email: ren@csit.fsu.edu                                   */
/*                                                                       */
/* Based on the implementation by:                                       */
/*             Ashok Srinivasan (Apr 13, 1998)                           */
/*************************************************************************/
/*************************************************************************/


#include <cstdio>
#include <cstring>
#include <climits>
#include "primes_32.h"
#include "lcg.h"
#include "lcg_config.h"


unsigned long int LCG::LCG_NGENS = 0;


LCG::LCG()
{
    rng_type = SPRNG_LCG;
    init_seed = 0;
    prime = 0;
    prime_position = 0;
    prime_next = 0;
    parameter = 0;
    gentype = CONFIG.GENTYPE;

#if defined(LONG_SPRNG)
    seed = 0;
    multiplier = 0;
#else
    seed[0] = 0;
    seed[1] = 1;
    memset(multiplier, 0, sizeof(multiplier));
#endif

    ++LCG_NGENS;
}


LCG::~LCG()
{
    --LCG_NGENS;
}


#if defined(LONG_SPRNG)
unsigned long int LCG::multiply(const unsigned long int a, const unsigned long int b, const unsigned long int c) const
{
    unsigned long int res = a * b;
    res += c;
    res &= 0xffffffffffffL;
    return res;
}
#else
void LCG::multiply(int * const a, const int * const b, const int c) const
{
    int s[4], res[4];

    s[0] = a[1] & 4095;
    s[1] = a[1] >> 12;
    s[2] = a[0] & 4095;
    s[3] = (unsigned int)a[0] >> 12;

    res[0] = b[0] * s[0];
    res[1] = b[1] * s[0] + b[0] * s[1];
    res[2] = b[2] * s[0] + b[1] * s[1] + b[0] * s[2];
    res[3] = b[3] * s[0] + b[2] * s[1] + b[1] * s[2] + b[0] * s[3];

    a[0] = ((unsigned int)a[1] >> 24) + res[2] + ((unsigned int)res[1] >> 12) + (res[3] << 12);
    a[1] = res[0] + ((res[1] & 4095) << 12) + c;

    a[0] &= 16777215;
    a[1] &= 16777215;
}
#endif


/*!
 *  Gives back one generator (node gennum) with updated spawning info.
 *  Should be called total_gen times, with different value
 *  of gennum in [0,total_gen) each call
 */
int LCG::init_rng(int gn, int tg, int s, int m)
{
    if (tg <= 0) {
        printf("ERROR: total_gen out of range, %d\n", tg);
        tg = 1;
    }
    prime_next = tg;

    // NOTE: verify these checks
    if (gn < 0 || gn >= tg || gn >= CONFIG.LCG_MAX_STREAMS) {
        printf("ERROR: gennum out of range, %d\n", gn);
        return -1;
    }
    prime_position = gn;
    const int need = 1;
    getprime_32(need, &prime, prime_position);

    if (m < 0 || m >= CONFIG.NPARAMS) {
        printf("ERROR: multiplier out of range, %d\n", m);
        m = 0;
    }
    parameter = m;

    init_seed = s & 0x7fffffff; // only 31 LSB of seed considered

#if defined(LONG_SPRNG)
    multiplier = CONFIG.MULT[parameter];

    seed = CONFIG.INIT_SEED ^ ((unsigned long int)init_seed << 16);

    if (prime == 0)
        seed |= 1;
#else
    memcpy(multiplier, CONFIG.MULT[parameter], sizeof(multiplier));

    seed[0] = CONFIG.INIT_SEED[0] ^ (((unsigned long int)init_seed >> 8) & 0xffffff);
    seed[1] = CONFIG.INIT_SEED[1] ^ (((unsigned long int)init_seed << 16) & 0xff0000);

    printf("seed[0]\t%d\n", seed[0]);
    printf("seed[1]\t%d\n", seed[1]);

    if (prime == 0)
        seed[1] |= 1;
#endif

    for (int i = 0; i < (CONFIG.LCG_RUNUP * prime_position); ++i)
        get_rn_dbl();
 
    return 0;
}


/*!
 *  On machines with 32 bit integers, the Cray's 48 bit integer math
 *  is duplicated by breaking the problem into steps.
 *  The algorithm is linear congruential.
 *  M is the multiplier and S is the current seed.
 *  The 31 High order bits out of the 48 bits are returned.
 */
int LCG::get_rn_int()
{
#if defined(LONG_SPRNG)
    seed = multiply(seed, multiplier, prime);
    return (int)(seed >> 17);
#else
    multiply(seed, multiplier, prime);
    return (seed[0] << 7) | ((unsigned int)seed[1] >> 17);
#endif
}


float LCG::get_rn_flt()
{
    return (float)get_rn_dbl();
}


double LCG::get_rn_dbl()
{
#if defined(LONG_SPRNG)
    seed = multiply(seed, multiplier, prime);
    return (double)seed * CONFIG.TWO_M48;
#else
    multiply(seed, multiplier, prime);
    return (double)seed[0] * CONFIG.TWO_M24 + (double)seed[1] * CONFIG.TWO_M48;
#endif
}


int LCG::get_seed_rng() const { return init_seed; }

unsigned long int LCG::get_ngens() const { return LCG_NGENS; }


#if defined(DEBUG)
int LCG::get_prime() const { return prime;}
#if defined(LONG_SPRNG)
unsigned long int LCG::get_seed() const { return seed; }
unsigned long int LCG::get_multiplier() const { return multiplier; }
#else
int LCG::get_seed() const { return seed[0]; }
int LCG::get_multiplier() const { return multiplier[0]; }
#endif
#endif


/***********************************************************************************
* SPRNG (c) 2014 by Florida State University                                       *
*                                                                                  *
* SPRNG is licensed under a                                                        *
* Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License. *
*                                                                                  *
* You should have received a copy of the license along with this                   *
* work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.           *
************************************************************************************/

