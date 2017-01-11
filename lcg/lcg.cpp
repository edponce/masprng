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


#include <stdio.h>
#include <string.h>
#include "lcg.h"
#include "lcg_globals.h"
#include "primes_32.h"


int LCG::LCG_NGENS = 0;


/*!
 *  \brief Constructor (no parameters)
 */
LCG::LCG()
{
    gentype = GLOBALS.GENTYPE;
    rng_type = SPRNG_LCG;
    init_seed = 0;
    prime = 0;
    prime_position = 0;
    prime_next = 0;
    parameter = 0;
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


/*!
 *  \brief Destructor
 */
LCG::~LCG()
{
    --LCG_NGENS;
}


#if defined(LONG_SPRNG)
/*!
 *  \brief LCG multiply 48-bits using 64-bits.
 */
unsigned long int LCG::multiply(const unsigned long int a, const unsigned long int b, const unsigned long int c) const
{
    unsigned long int res = a * b;

    res += c;
    res &= 0xFFFFFFFFFFFFUL;

    return res;
}
#else
/*!
 *  \brief LCG multiply 48-bits using 32-bits.
 */
void LCG::multiply(int * const a, const int * const b, const int c) const
{
    int s[4], res[4];

    s[0] = a[1] & 4095U;
    s[1] = (unsigned int)a[1] >> 0xC;
    s[2] = a[0] & 4095U;
    s[3] = (unsigned int)a[0] >> 0xC;

    for (int i = 0; i < 4; ++i) {
        res[i] = b[0] * s[i];
        for (int j = 0; j < i; ++j) {
            const int vmul = b[i-j] * s[j];
            res[i] += vmul;
        }
    }

    a[0] = ((unsigned int)a[1] >> 0x18) + res[2] + ((unsigned int)res[1] >> 0xC) + ((unsigned int)res[3] << 0xC);
    a[0] &= 16777215U;

    a[1] = res[0] + ((unsigned int)(res[1] & 4095U) << 0xC) + c;
    a[1] &= 16777215U;
}
#endif


/*!
 *  \brief Initialize RNG
 *
 *  Gives back one generator (node gennum) with updated spawning info.
 *  Should be called total_gen times, with different value
 *  of gennum in [0,total_gen) each call.
 */
int LCG::init_rng(int gn, int tg, int s, int m)
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
    prime_position = gn;
    getprime_32(1, &prime, prime_position);

    if (m < 0 || m >= GLOBALS.NPARAMS) {
        printf("ERROR: multiplier out of range, %d\n", m);
        m = 0;
    }
    parameter = m;

    init_seed = s & 0x7FFFFFFFUL;

#if defined(LONG_SPRNG)
    multiplier = GLOBALS.MULT[parameter];

    seed = GLOBALS.INIT_SEED ^ ((unsigned long int)init_seed << 16);

    if (prime == 0)
        seed |= 0x1;
#else
    memcpy(multiplier, GLOBALS.MULT[parameter], sizeof(multiplier));

    seed[0] = GLOBALS.INIT_SEED[0] ^ (((unsigned int)init_seed >> 0x8) & 0xFFFFFFU);
    seed[1] = GLOBALS.INIT_SEED[1] ^ (((unsigned int)init_seed << 0x10) & 0xFF0000U);

    if (prime == 0)
        seed[1] |= 0x1;
#endif

    for (int i = 0; i < (GLOBALS.LCG_RUNUP * prime_position); ++i)
        get_rn_dbl();
 
    return 0;
}


/*!
 *  The high 31-bits out of the 48-bits are returned.
 */
int LCG::get_rn_int()
{
#if defined(LONG_SPRNG)
    seed = multiply(seed, multiplier, prime);

    return (int)(seed >> 0x11);
#else
    multiply(seed, multiplier, prime);

    return (seed[0] << 0x7) | (seed[1] >> 0x11);
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

    return (double)seed * GLOBALS.TWO_M48;
#else
    multiply(seed, multiplier, prime);

    return (double)seed[0] * GLOBALS.TWO_M24 + (double)seed[1] * GLOBALS.TWO_M48;
#endif
}


int LCG::get_seed_rng() const
{ return init_seed; }


int LCG::get_ngens() const
{ return LCG_NGENS; }


#if defined(DEBUG)
int LCG::get_prime() const
{ return prime; }

# if defined(LONG_SPRNG)
unsigned long int LCG::get_seed() const
{ return seed; }

unsigned long int LCG::get_multiplier() const
{ return multiplier; }

# else
int LCG::get_seed() const
{ return seed[0]; }

int LCG::get_multiplier() const
{ return multiplier[0]; }
# endif
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

