/*************************************************************************/
/*************************************************************************/
/*             Parallel 48 bit Linear Congruential Generator             */
/*                                                                       */ 
/* Modified by: Eduardo Ponce                                            */
/*             The University of Tennessee, Knoxville                    */
/*             Email: eponcemo@utk.edu (Jan 2017)                        */
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
#include <limits.h>
#include "primes_32.h"
#include "lcg.h"


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
    seed = CONFIG.INIT_SEED;
    multiplier = 0;
    for (int i = 0; i < CONFIG.NPARAMS; ++i)
        mults_g[i] = CONFIG.MULT[i];
    multiplier_g = 0;
#else
    // NOTE: original version sets to 0 and 1 but they are overwritten.
    seed[0] = CONFIG.INIT_SEED1;
    seed[1] = CONFIG.INIT_SEED2;
    multiplier = NULL;
#endif

    ++LCG_NGENS;
}


LCG::~LCG()
{
    --LCG_NGENS;
}


#if defined(LONG_SPRNG)
inline unsigned long int LCG::multiply_48_64(unsigned long int a, int b)
{
    unsigned long int c = seed;

    c *= a;
    c += b;
    c &= CONFIG.LSB48;

    return c;
}
#endif


/*!
 *  Gives back one generator (node gennum) with updated spawning info.
 *  Should be called total_gen times, with different value
 *  of gennum in [0,total_gen) each call
 */
int LCG::init_rng(int gn, int tg, int s, int m)
{
    int i;
    int need = 1;

    s &= 0x7fffffff; // only 31 LSB of seed considered
    init_seed = s;

    if (tg <= 0) {
        printf("ERROR: total_gen out of range, %d\n", tg);
        tg = 1;
    }
    prime_next = tg;

    if (gn < 0 || gn >= tg || gn >= CONFIG.LCG_MAX_STREAMS) {
        printf("ERROR: gennum out of range, %d\n", gn);
        return -1;
    }
    prime_position = gn;

    getprime_32(need, &prime, gn);
    getprime_32(need, &prime, gn);

    if (m < 0 || m >= CONFIG.NPARAMS) {
        printf("ERROR: multiplier out of range, %d\n", m);
        m = 0;
    }
    parameter = m;

#if defined(LONG_SPRNG)
    if (multiplier == 0)
        multiplier = mults_g[m];

    seed ^= (unsigned long int)s << 16;

    if (prime == 0)
        seed |= 1;
#endif

    for (i = 0; i < (CONFIG.LCG_RUNUP * gn); ++i)
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
    seed = multiply_48_64(multiplier, prime);
    return (int)(seed >> 17);
}


float LCG::get_rn_flt()
{
    return (float)get_rn_dbl();
}


double LCG::get_rn_dbl()
{
    seed = multiply_48_64(multiplier, prime);
    return (double)seed * CONFIG.TWO_M48;
}


int LCG::get_seed_rng() { return init_seed; }

unsigned long int LCG::get_ngens() { return LCG_NGENS; }


// NOTE: debug purposes
int LCG::get_prime() { return prime;}
#if defined(LONG_SPRNG)
unsigned long int LCG::get_seed() { return seed; }
unsigned long int LCG::get_multiplier() { return multiplier; }
#else
int LCG::get_seed() { return seed[0]; }
int LCG::get_multiplier() { return *multiplier; }
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

