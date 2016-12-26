#include <stdio.h>
#include <limits.h>
#include "primes_32.h"
#include "lcg_simd.h"


// Global variables
#if defined(LONG_SPRNG)
static unsigned long int mults_g[MAX_MULTS] = {MULT1, MULT2, MULT3, MULT4, MULT5, MULT6, MULT7};
static unsigned long int multiplier_g = 0;
#else

static int mults_g[MAX_MULTS][4] = {{0x175,0xe7b,0x5a2,0x287},{0x66d,0xece,0x5de,0x000},
	                         {0x265,0x605,0xc44,0x3ea},{0x9f5,0x9cc,0x142,0x1ee},
	                         {0xbbd,0xeb4,0xb38,0x275},{0x605,0xb08,0xa9c,0x739},
	                         {0x5f5,0xcc2,0x8d7,0x322}};
static int *multiplier_g = NULL;
#endif


#if defined(SIMD_MODE)
unsigned int VLCG::LCG_NGENS = 0;


VLCG::VLCG()
{
    rng_type = SPRNG_LCG;
    simd_set_zero(&init_seed);
    simd_set_zero(&prime);
    simd_set_zero(&prime_position);
    simd_set_zero(&parameter);

#if defined(LONG_SPRNG) 
    seed = simd_set(INIT_SEED, INIT_SEED);
    simd_set_zero(&multiplier);
#else
    seed = simd_set(16651885L, 2868876L);
    multiplier = simd_set_64(NULL);
#endif

    ++LCG_NGENS;
}


VLCG::~VLCG()
{
    --LCG_NGENS;
}


#if defined(USE_SSE)

// Global constants
static SIMD_INT vmsk_lsb1[3];
static SIMD_INT vmsk_lh64[3];
static SIMD_INT vmsk_hi64;
static SIMD_INT vmsk_lsb48;
static SIMD_INT vmsk_seed;
static SIMD_INT vmult_shf;

#if defined(LONG_SPRNG)
/*
 *  Perform 64-bit integer multiplication using 32-bit integers
 *  x64 * y64 = (xl32 * yl32) + (xl32 * yh32 + xh32 * yl32) * 2^32
 */
static inline void multiply(SIMD_INT * const c, const SIMD_INT a, const SIMD_INT b)
{
    // NOTE: require at least SSE 4.1 for simd_mullo_epi32()
    SIMD_INT st, vtmp, u;

    vmult_shf = simd_shuffle_32(a, 0xb1); // shuffle multiplier 
    st = simd_mullo_32(*c, vmult_shf); // xl * yh, xh * yl
    vtmp = simd_sll_64(st, 0x20); // shift << 32 
    st = simd_add_64(st, vtmp); // s + t 
    st = simd_and(st, vmsk_hi64); // (s + t) & 0xFFFFFFFF00000000
    u = simd_mul_u32(*c, a); // xl * yl
    *c = simd_add_64(u, st); // u + s + t 
    *c = simd_add_64(*c, b);    // seed += prime
    *c = simd_and(*c, vmsk_lsb48); // seed &= LSB48
}
#endif // LONG_SPRNG


// Initialize SIMD RNG
int VLCG::init_rng(int *s, int *m)
{
    // Set vector masks
    vmsk_lsb1[0] = simd_set(0x1UL); // all 
    vmsk_lsb1[1] = simd_set(0x0ULL, 0x1ULL); // first
    vmsk_lsb1[2] = simd_set(0x1ULL, 0x0ULL); // second
    vmsk_lh64[0] = simd_set(0xFFFFFFFFFFFFFFFFULL); // all 
    vmsk_lh64[1] = simd_set(0x0ULL, 0xFFFFFFFFFFFFFFFFULL); // first
    vmsk_lh64[2] = simd_set(0xFFFFFFFFFFFFFFFFULL, 0x0ULL); // second
    vmsk_hi64 = simd_set(0xFFFFFFFF00000000ULL); // all
    vmsk_lsb48 = simd_set(LSB48); // all
    vmsk_seed = simd_set(0x7FFFFFFFULL); // only 31 LSB of seed considered

    int i;
    int need = SIMD_NUM_STREAMS;
    int primes[SIMD_NUM_STREAMS];
    SIMD_INT vs;

    vs = simd_set(s[1], s[0]);
    vs = simd_and(vs, vmsk_seed); // s &= 0x7FFFFFFF
    init_seed = vs;

    int gn = 0;
    getprime_32(need, primes, gn);
    prime = simd_set(primes[0], primes[0]);
    prime_position = simd_set(gn, gn);

    for (i = 0; i < SIMD_NUM_STREAMS; ++i)
        if (m[i] < 0 || m[i] >= MAX_MULTS)
            m[i] = 0;
    parameter = simd_set(m[1], m[0]);

#if defined(LONG_SPRNG)
    if (simd_test_zero(multiplier, vmsk_lh64[0]) == 1) { // all streams have multiplier = 0
        multiplier = simd_set(mults_g[m[1]], mults_g[m[0]]);
        vmult_shf = simd_shuffle_32(multiplier, 0xB1); // shuffle multiplier 
    }

    vs = simd_sll_64(vs, 0x10); // seed << 16
    seed = simd_xor(seed, vs); // seed ^= (s << 16)

    // NOTE: require at least SSE 4.1 for simd_test_zero()
    if (simd_test_zero(prime, vmsk_lh64[0]) == 1) // all streams have prime = 0
        seed = simd_or(seed, vmsk_lsb1[0]);
    else if (simd_test_zero(prime, vmsk_lh64[1]) == 1) // first stream has prime = 0
        seed = simd_or(seed, vmsk_lsb1[1]);
    else if (simd_test_zero(prime, vmsk_lh64[2]) == 1) // second stream has prime = 0
        seed = simd_or(seed, vmsk_lsb1[2]);
#endif // LONG_SPRNG

    // Run generator
    // NOTE: the factor is not gn, but prime_position. How to do with vector?
    for (i = 0; i < (LCG_RUNUP * gn); ++i)
        get_rn_dbl();
 
    return 0;
}


// SIMD multiply and new seed
SIMD_INT VLCG::get_rn_int()
{
    multiply(&seed, multiplier, prime);
    return simd_srl_64(seed, 0x11); // seed >> 17
}


// SIMD multiply and new seed
SIMD_DP VLCG::get_rn_dbl()
{
    unsigned long int lseed[2] __attribute__ ((aligned(SIMD_ALIGN)));
    double seedd[2];
    SIMD_DP vseedd;
    SIMD_DP vrng = simd_set(RNG_LONG64_DBL);

    multiply(&seed, multiplier, prime);

    // NOTE: casting done with CPU, bad!!
    simd_store(lseed, seed);
    seedd[0] = lseed[0];
    seedd[1] = lseed[1];
    vseedd = simd_load(seedd);

    return simd_mul_pd(vseedd, vrng); // seed * constant
}


// SIMD multiply and new seed
SIMD_SP VLCG::get_rn_flt()
{
    unsigned long int lseed[2] __attribute__ ((aligned(SIMD_ALIGN)));
    float seedd[2];
    SIMD_SP vseedd;
    SIMD_SP vrng = simd_set((float)RNG_LONG64_DBL);

    multiply(&seed, multiplier, prime);

    // NOTE: casting done with CPU, bad!!
    simd_store(lseed, seed);
    seedd[0] = lseed[0];
    seedd[1] = lseed[1];
    vseedd = simd_load(seedd);

    return simd_mul_ps(vseedd, vrng); // seed * constant
    //return simd_cvt_pd2ps(get_rn_dbl());
}


// NOTE: debug purposes
SIMD_INT VLCG::get_seed() {return seed;}
SIMD_INT VLCG::get_prime() {return prime;}
SIMD_INT VLCG::get_multiplier() {return multiplier;}

#endif // USE_SSE
#endif // SIMD_MODE

