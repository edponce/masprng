#include <stdio.h>
#include <limits.h>
#include "primes_32.h"
#include "lcg.h"


#if defined(LONG_SPRNG)
static unsigned long int mults_g[MAX_MULTS] = {MULT1, MULT2, MULT3, MULT4, MULT5, MULT6, MULT7};
static unsigned long int multiplier_g = 0;
#else

static int mults_g[MAX_MULTS][4] = {MULT1, MULT2, MULT3, MULT4, MULT5, MULT6, MULT7};
static int *multiplier_g = NULL;
#endif


unsigned int LCG::LCG_NGENS = 0;


LCG::LCG()
{
    rng_type = SPRNG_LCG;
    init_seed = 0;
    prime = 0;
    prime_position = 0;
    parameter = 0;

#if defined(LONG_SPRNG)
    seed = INIT_SEED;
    multiplier = 0;
#else
    // NOTE: original version sets to 0 and 1 but they are overwritten.
    seed[0] = INIT_SEED1;
    seed[1] = INIT_SEED2;
    multiplier = NULL;
#endif

    ++LCG_NGENS;
}


LCG::~LCG()
{
    --LCG_NGENS;
}


#if defined(LONG_SPRNG)
static inline void multiply(unsigned long int * const c, unsigned long int const a, int const b)
{
    *c *= a;
    *c += b;
    *c &= LSB48;
}
#endif


int LCG::init_rng(int s, int m)
{
    int i;
    int need = 1;

    s &= 0x7fffffff; // only 31 LSB of seed considered
    init_seed = s;

    // NOTE: parameterized, the prime offset value needs to change
    int gn = 0;
    getprime_32(need, &prime, gn);
    prime_position = gn;

    if (m < 0 || m >= MAX_MULTS)
        m = 0;
    parameter = m;

#if defined(LONG_SPRNG)
    if (multiplier == 0)
        multiplier = mults_g[m];

    seed ^= (unsigned long int)s << 16;

    if (prime == 0)
        seed |= 1;
#endif

    for (i = 0; i < (LCG_RUNUP * prime_position); ++i)
        get_rn_dbl();
 
    return 0;
}


int LCG::get_rn_int()
{
    multiply(&seed, multiplier, prime);
    return (int)(seed >> 17);
}


float LCG::get_rn_flt()
{
    return (float)get_rn_dbl();
}


double LCG::get_rn_dbl()
{
    multiply(&seed, multiplier, prime);
    return (double)seed * RNG_LONG64_DBL;
}


int LCG::get_seed_rng() {return init_seed;}


// NOTE: debug purposes
int LCG::get_prime() {return prime;}
#if defined(LONG_SPRNG)
unsigned long int LCG::get_seed() {return seed;}
unsigned long int LCG::get_multiplier() {return multiplier;}
#else
int LCG::get_seed() {return seed[0];}
int LCG::get_multiplier() {return *multiplier;}
#endif

