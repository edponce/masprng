#include <stdio.h>
#include <limits.h>
#include "primes_32.h"
#include "lcg.h"


#if defined(LONG_SPRNG)
#define INIT_SEED 0x2bc68cfe166dL
#define MSB_SET 0x3ff0000000000000L
#define LSB48 0xffffffffffffL
#define AN1 0xdadf0ac00001L
#define AN2 0xfefd7a400001L
#define AN3 0x6417b5c00001L
#define AN4 0xcf9f72c00001L
#define AN5 0xbdf07b400001L
#define AN6 0xf33747c00001L
#define AN7 0xcbe632c00001L
#define PMULT1 0xa42c22700000L
#define PMULT2 0xfa858cb00000L
#define PMULT3 0xd0c4ef00000L
#define PMULT4 0xc3cc8e300000L
#define PMULT5 0x11bdbe700000L
#define PMULT6 0xb0f0e9f00000L
#define PMULT7 0x6407de700000L

/*
 *  Multiplier factors
 */
#define MAX_MULTS 7 /* max number of multiplier factors */
#define MULT1 0x2875a2e7b175L   /* 44485709377909  */
#define MULT2 0x5deece66dL      /* 1575931494      */
#define MULT3 0x3eac44605265L   /* 68909602460261  */
#define MULT4 0x275b38eb4bbdL   /* 4327250451645   */
#define MULT5 0x1ee1429cc9f5L   /* 33952834046453  */
#define MULT6 0x739a9cb08605L   /* 127107890972165 */
#define MULT7 0x3228d7cc25f5L   /* 55151000561141  */
#endif

#define TWO_M24 5.96046447753906234e-8
#define TWO_M48 3.5527136788005008323e-15

#define LCG_RUNUP 29

#if defined(LittleEndian)
#define MSB 1
#else
#define MSB 0
#endif
#define LSB (1-MSB)



#define RNG_LONG64_DBL 3.5527136788005008e-15



// Global variables
#if defined(LONG_SPRNG)
unsigned long int mults_g[MAX_MULTS] = {MULT1, MULT2, MULT3, MULT4, MULT5, MULT6, MULT7};
unsigned long int multiplier_g = 0;
#else

int mults_g[MAX_MULTS][4] = {{0x175,0xe7b,0x5a2,0x287},{0x66d,0xece,0x5de,0x000},
	                         {0x265,0x605,0xc44,0x3ea},{0x9f5,0x9cc,0x142,0x1ee},
	                         {0xbbd,0xeb4,0xb38,0x275},{0x605,0xb08,0xa9c,0x739},
	                         {0x5f5,0xcc2,0x8d7,0x322}};
int *multiplier_g = NULL;
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
    seed[0] = 2868876;  // 0
    seed[1] = 16651885; // 1
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
    *c *= a; //mult_48_64(&seed,&multiplier,&seed);
    *c += b;
    *c &= LSB48;
}
#endif


// Initialize RNG
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


// Multiply and new seed
int LCG::get_rn_int()
{
    multiply(&seed, multiplier, prime);
    return (int)(seed >> 17);
}


// Multiply and new seed
float LCG::get_rn_flt()
{
    return (float)get_rn_dbl();
}


// Multiply and new seed
double LCG::get_rn_dbl()
{
    multiply(&seed, multiplier, prime);
    return (double)seed * RNG_LONG64_DBL;
}


// NOTE: debug purposes
unsigned long int LCG::get_seed() {return seed;}
int LCG::get_prime() {return prime;}
unsigned long int LCG::get_multiplier() {return multiplier;}

