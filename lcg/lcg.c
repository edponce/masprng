#include <stdio.h>
#include "primes_32.h"
#include "lcg.h"


// Global variables
unsigned long int mults_g[MAX_MULTIPLIERS] = {MULT1, MULT2, MULT3, MULT4, MULT5, MULT6, MULT7};
unsigned long int multiplier_g = 0;

// Initialize RNG
int init_rng(unsigned long int *seed, unsigned long int *mult, unsigned int *prime, int s, int m)
{
    int i;
    int offs = 0; // NOTE: gn
    int need = 1;
    int prime_position = offs;

    s &= 0x7FFFFFFF; // only 31 LSB of seed considered

    getprime_32(need, (int *)prime, offs);

    *seed = INIT_SEED ^ (unsigned long int)s << 16;

    if (m < 0 || m > MAX_MULTIPLIERS)
        m = 0;
    *mult = mults_g[m];

    if (*prime == 0)
        *seed |= 1;

    for (i = 0; i < (LCGRUNUP * prime_position); ++i)
        get_rn_dbl(seed, *mult, *prime);
 
    return 0;
}


// Multiply and new seed
int get_rn_int(unsigned long int *seed, const unsigned long int multiplier, const unsigned int prime)
{
    *seed *= multiplier; //mult_48_64(&seed,&multiplier,&seed);
    *seed += prime;
    *seed &= LSB48;

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
    *seed *= multiplier; //mult_48_64(&seed,&multiplier,&seed);
    *seed += prime;
    *seed &= LSB48;

    return (double)(*seed * RNG_LONG64_DBL);
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

// Initialize SIMD RNG
int init_vrng(SIMD_INT *vseed, SIMD_INT *vmult, SIMD_INT *vprime, int *s, int *m)
{
    // Vector masks
    vmsk_lsb1[0] = simd_set1_64(0x1ULL); // all 
    vmsk_lsb1[1] = simd_set_64(0x0ULL, 0x1ULL); // first
    vmsk_lsb1[2] = simd_set_64(0x1ULL, 0x0ULL); // second
    vmsk_lh64[0] = simd_set1_64(0xFFFFFFFFFFFFFFFFULL); // all 
    vmsk_lh64[1] = simd_set_64(0x0ULL, 0xFFFFFFFFFFFFFFFFULL); // first
    vmsk_lh64[2] = simd_set_64(0xFFFFFFFFFFFFFFFFULL, 0x0ULL); // second
    vmsk_hi64 = simd_set1_64(0xFFFFFFFF00000000ULL); // all
    vmsk_lsb48 = simd_set1_64(LSB48); // all
    vmsk_seed = simd_set1_64(0x7FFFFFFFULL); // only 31 LSB of seed considered

    int i;
    int offs = 0;
    int need = 1;
    int prime = 0;
    int prime_position = offs;
    SIMD_INT vs;

    vs = simd_set_64(s[1], s[0]);
    vs = simd_and(vs, vmsk_seed); // s &= 0x7FFFFFFF

    getprime_32(need, &prime, offs);

    // NOTE: need to allow different value for streams
    *vprime = simd_set_64(prime, prime);
    *vseed = simd_set_64(INIT_SEED, INIT_SEED);

    vs = simd_sll_64(vs, 0x10); // seed << 16
    *vseed = simd_xor(*vseed, vs); // seed ^= (s << 16)
    *vmult = simd_set_64(mults_g[m[1]], mults_g[m[0]]); // NOTE: parameterize

    // Vector masks
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

    // Run generator
    for (i = 0; i < (LCGRUNUP * prime_position); ++i)
        get_vrn_dbl(vseed, *vmult, *vprime);
 
    return 0;
}


// SIMD multiply and new seed
SIMD_INT get_vrn_int(SIMD_INT *vseed, const SIMD_INT vmult, const SIMD_INT vprime)
{
    SIMD_INT st, vtmp, u, vrng;

#if defined(__SSE4_1__)
    vmult_shf = simd_shuffle_32(vmult, 0xB1); // shuffle multiplier 
    st = simd_mullo_32(*vseed, vmult_shf); // xl * yh, xh * yl
#else
    printf("ERROR: require at least SSE4.1\n");
    simd_set_zero(&vrng);
    return vrng;
#endif

    vtmp = simd_sll_64(st, 0x20); // shift << 32 
    st = simd_add_64(st, vtmp); // s + t 
    st = simd_and(st, vmsk_hi64); // (s + t) & 0xFFFFFFFF00000000
    u = simd_mul_u32(*vseed, vmult); // xl * yl
    *vseed = simd_add_64(u, st); // u + s + t 
    *vseed = simd_add_64(*vseed, vprime);    // seed += prime
    *vseed = simd_and(*vseed, vmsk_lsb48); // seed &= LSB48
    vrng = simd_srl_64(*vseed, 0x11); // seed >> 17

    return vrng;
}


// SIMD multiply and new seed
SIMD_DP get_vrn_dbl(SIMD_INT *vseed, const SIMD_INT vmult, const SIMD_INT vprime)
{
    SIMD_INT st, vtmp, u;
    SIMD_DP vseedd, vrng;
    const double val[2] = {RNG_LONG64_DBL, RNG_LONG64_DBL};
    unsigned long int seed[2] __attribute__ ((aligned(SIMD_ALIGN)));
    double seedd[2];

#if defined(__SSE4_1__)
    vmult_shf = simd_shuffle_32(vmult, 0xB1); // shuffle multiplier 
    st = simd_mullo_32(*vseed, vmult_shf); // xl * yh, xh * yl
#else
    printf("ERROR: require at least SSE4.1\n");
    simd_set_zero(&vrng);
    return vrng;
#endif

    vtmp = simd_sll_64(st, 0x20); // shift << 32 
    st = simd_add_64(st, vtmp); // s + t 
    st = simd_and(st, vmsk_hi64); // (s + t) & 0xFFFFFFFF00000000
    u = simd_mul_u32(*vseed, vmult); // xl * yl
    *vseed = simd_add_64(u, st); // u + s + t 
    *vseed = simd_add_64(*vseed, vprime);    // seed += prime
    *vseed = simd_and(*vseed, vmsk_lsb48); // seed &= LSB48
    vrng = simd_load(val);
    simd_store(seed, *vseed);
    seedd[0] = (double)seed[0];
    seedd[1] = (double)seed[1];
    vseedd = simd_load((double *)seedd);
    vrng = simd_mul_pd(vseedd, vrng); // seed * constant

    return vrng;
}


// SIMD multiply and new seed
SIMD_SP get_vrn_flt(SIMD_INT *vseed, const SIMD_INT vmult, const SIMD_INT vprime)
{
    return simd_cvt_pd2ps(get_vrn_dbl(vseed, vmult, vprime));
}
#endif
#endif // SIMD_MODE

