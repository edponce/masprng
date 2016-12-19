#include <stdio.h>
#include <limits.h>
#include "primes_32.h"
#include "lcg.h"


#ifdef LONG_SPRNG
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

#define LCGRUNUP 29

#ifdef LittleEndian
#define MSB 1
#else
#define MSB 0
#endif
#define LSB (1-MSB)



#define RNG_LONG64_DBL 3.5527136788005008e-15

#define MAX_MULTIPLIERS 7 /* max number of multiplier factors */


// Global variables
#ifdef LONG_SPRNG
unsigned long int mults_g[MAX_MULTIPLIERS] = {MULT1, MULT2, MULT3, MULT4, MULT5, MULT6, MULT7};
unsigned long int multiplier_g = 0;
#else

int mults_g[MAX_MULTIPLIERS][4] = {{0x175,0xe7b,0x5a2,0x287},{0x66d,0xece,0x5de,0x000},
		                         {0x265,0x605,0xc44,0x3ea},{0x9f5,0x9cc,0x142,0x1ee},
		                         {0xbbd,0xeb4,0xb38,0x275},{0x605,0xb08,0xa9c,0x739},
		                         {0x5f5,0xcc2,0x8d7,0x322}};
int *multiplier_g = NULL;
#endif


#ifdef LONG_SPRNG
static void multiply(unsigned long int * const seed, unsigned long int const multiplier, unsigned int const prime)
{
    *seed *= multiplier; //mult_48_64(&seed,&multiplier,&seed);
    *seed += prime;
    *seed &= LSB48;
}
#endif


// Initialize RNG
int init_rng(unsigned long int *seed, unsigned long int *mult, unsigned int *prime, int s, int m)
{
    int i;
    int offs = 0; // NOTE: gn
    int need = 1;
    int prime_position = offs;

    s &= 0x7FFFFFFF; // only 31 LSB of seed considered

    getprime_32(need, (int *)prime, offs);

#ifdef LONG_SPRNG
    *seed = INIT_SEED ^ (unsigned long int)s << 16;

    if (m < 0 || m > MAX_MULTIPLIERS)
        m = 0;
    *mult = mults_g[m];

    if (*prime == 0)
        *seed |= 1;
#endif

    for (i = 0; i < (LCGRUNUP * prime_position); ++i)
        get_rn_dbl(seed, *mult, *prime);
 
    return 0;
}


// Multiply and new seed
int get_rn_int(unsigned long int *seed, const unsigned long int multiplier, const unsigned int prime)
{
    multiply(seed, multiplier, prime);
    return (int)(*seed >> 17);
}


// Multiply and new seed
float get_rn_flt(unsigned long int *seed, const unsigned long int multiplier, const unsigned int prime)
{
    return (float)get_rn_dbl(seed, multiplier, prime);
}


// Multiply and new seed
double get_rn_dbl(unsigned long int *seed, const unsigned long int multiplier, const unsigned int prime)
{
    multiply(seed, multiplier, prime);
    return *seed * RNG_LONG64_DBL;
}


#if defined(SIMD_MODE)
#if defined(USE_SSE)

// Global constants
static SIMD_INT vmsk_lsb1[3];
static SIMD_INT vmsk_lh64[3];
static SIMD_INT vmsk_hi64;
static SIMD_INT vmsk_lsb48;
static SIMD_INT vmsk_seed;
static SIMD_INT vmult_shf;

#ifdef LONG_SPRNG
/*
 *  Perform 64-bit integer multiplication using 32-bit integers
 *  x64 * y64 = (xl32 * yl32) + (xl32 * yh32 + xh32 * yl32) * 2^32
 */
static void multiply(SIMD_INT * const vseed, const SIMD_INT vmult, const SIMD_INT vprime)
{
#if defined(__SSE4_1__)
    SIMD_INT st, vtmp, u;

    vmult_shf = simd_shuffle_32(vmult, 0xB1); // shuffle multiplier 
    st = simd_mullo_32(*vseed, vmult_shf); // xl * yh, xh * yl
    vtmp = simd_sll_64(st, 0x20); // shift << 32 
    st = simd_add_64(st, vtmp); // s + t 
    st = simd_and(st, vmsk_hi64); // (s + t) & 0xFFFFFFFF00000000
    u = simd_mul_u32(*vseed, vmult); // xl * yl
    *vseed = simd_add_64(u, st); // u + s + t 
    *vseed = simd_add_64(*vseed, vprime);    // seed += prime
    *vseed = simd_and(*vseed, vmsk_lsb48); // seed &= LSB48
#else
    printf("ERROR: require at least SSE4.1\n");
    simd_set_zero(vseed);
#endif
}
#endif // LONG_SPRNG


// Initialize SIMD RNG
int init_vrng(SIMD_INT *vseed, SIMD_INT *vmult, SIMD_INT *vprime, int *s, int *m)
{
    // Set vector masks
    vmsk_lsb1[0] = simd_set1(0x1UL); // all 
    vmsk_lsb1[1] = simd_set(0x0ULL, 0x1ULL); // first
    vmsk_lsb1[2] = simd_set(0x1ULL, 0x0ULL); // second
    vmsk_lh64[0] = simd_set1(0xFFFFFFFFFFFFFFFFULL); // all 
    vmsk_lh64[1] = simd_set(0x0ULL, 0xFFFFFFFFFFFFFFFFULL); // first
    vmsk_lh64[2] = simd_set(0xFFFFFFFFFFFFFFFFULL, 0x0ULL); // second
    vmsk_hi64 = simd_set1(0xFFFFFFFF00000000ULL); // all
    vmsk_lsb48 = simd_set1(LSB48); // all
    vmsk_seed = simd_set1(0x7FFFFFFFULL); // only 31 LSB of seed considered

    int i;
    int offs = 0;
    int need = 1;
    int prime = 0;
    int prime_position = offs;
    SIMD_INT vs;

    vs = simd_set(s[1], s[0]);
    vs = simd_and(vs, vmsk_seed); // s &= 0x7FFFFFFF

    getprime_32(need, &prime, offs);

    // NOTE: need to allow different value for streams
    *vprime = simd_set(prime, prime);

#ifdef LONG_SPRNG
    *vseed = simd_set(INIT_SEED, INIT_SEED);
    vs = simd_sll_64(vs, 0x10); // seed << 16
    *vseed = simd_xor(*vseed, vs); // seed ^= (s << 16)
    *vmult = simd_set(mults_g[m[1]], mults_g[m[0]]); // NOTE: parameterize
    vmult_shf = simd_shuffle_32(*vmult, 0xB1); // shuffle multiplier 

#if defined(__SSE4_1__)
    if (simd_test_zero(*vprime, vmsk_lh64[0]) == 1) // all streams have prime = 0
        *vseed = simd_or(*vseed, vmsk_lsb1[0]);
    else if (simd_test_zero(*vprime, vmsk_lh64[1]) == 1) // first stream has prime = 0
        *vseed = simd_or(*vseed, vmsk_lsb1[1]);
    else if (simd_test_zero(*vprime, vmsk_lh64[2]) == 1) // second stream has prime = 0
        *vseed = simd_or(*vseed, vmsk_lsb1[2]);
#else
    printf("ERROR: require at least SSE4.1\n");
    return -1;
#endif
#endif // LONG_SPRNG

    // Run generator
    for (i = 0; i < (LCGRUNUP * prime_position); ++i)
        get_vrn_dbl(vseed, *vmult, *vprime);
 
    return 0;
}


// SIMD multiply and new seed
SIMD_INT get_vrn_int(SIMD_INT *vseed, const SIMD_INT vmult, const SIMD_INT vprime)
{
    multiply(vseed, vmult, vprime);
    return simd_srl_64(*vseed, 0x11); // seed >> 17
}


// SIMD multiply and new seed
SIMD_DP get_vrn_dbl(SIMD_INT *vseed, const SIMD_INT vmult, const SIMD_INT vprime)
{
    unsigned long int seed[2] __attribute__ ((aligned(SIMD_ALIGN)));
    double seedd[2];
    SIMD_DP vseedd;
    SIMD_DP vrng = simd_set1(RNG_LONG64_DBL);

    multiply(vseed, vmult, vprime);

    // NOTE: casting done with CPU, bad!!
    simd_store(seed, *vseed);
    seedd[0] = seed[0];
    seedd[1] = seed[1];
    vseedd = simd_load(seedd);

    return simd_mul_pd(vseedd, vrng); // seed * constant
}


// SIMD multiply and new seed
SIMD_SP get_vrn_flt(SIMD_INT *vseed, const SIMD_INT vmult, const SIMD_INT vprime)
{
    unsigned long int seed[2] __attribute__ ((aligned(SIMD_ALIGN)));
    float seedd[2];
    SIMD_SP vseedd;
    SIMD_SP vrng = simd_set1((float)RNG_LONG64_DBL);

    multiply(vseed, vmult, vprime);

    // NOTE: casting done with CPU, bad!!
    simd_store(seed, *vseed);
    seedd[0] = seed[0];
    seedd[1] = seed[1];
    vseedd = simd_load(seedd);

    return simd_mul_ps(vseedd, vrng); // seed * constant
    //return simd_cvt_pd2ps(get_vrn_dbl(vseed, vmult, vprime));
}
#endif // USE_SSE
#endif // SIMD_MODE

