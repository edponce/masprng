#include <stdio.h>
#include "primes_32.h"
#include "vutils.h"
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
#if defined(USE_AVX)
// Global constants
static VECTOR_INT vmsk_lsb1[5];
static VECTOR_INT vmsk_lh64[5];
static VECTOR_INT vmsk_hi64;
static VECTOR_INT vmsk_lsb48;
static VECTOR_INT vmsk_seed;
static VECTOR_INT vmult_shf;

// Initialize SIMD RNG
int init_vrng(VECTOR_INT *vseed, VECTOR_INT *vmult, VECTOR_INT *vprime, int *s, int *m)
{
    // Vector masks
    vmsk_lsb1[0] = _mm256_set1_epi64x(0x1); // all
    vmsk_lsb1[1] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x1); // first
    vmsk_lsb1[2] = _mm256_set_epi64x(0x0, 0x0, 0x1, 0x0); // second
    vmsk_lsb1[3] = _mm256_set_epi64x(0x0, 0x1, 0x0, 0x0); // third
    vmsk_lsb1[4] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x1); // fourth
    vmsk_lh64[0] = _mm256_set1_epi64x(0xFFFFFFFFFFFFFFF); // all 
    vmsk_lh64[1] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0xFFFFFFFFFFFFFFFF); // first
    vmsk_lh64[2] = _mm256_set_epi64x(0x0, 0x0, 0xFFFFFFFFFFFFFFFF, 0x0); // second
    vmsk_lh64[3] = _mm256_set_epi64x(0x0, 0xFFFFFFFFFFFFFFFF, 0x0, 0x0); // third 
    vmsk_lh64[4] = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0, 0x0, 0x0); // fourth 
    vmsk_hi64 = _mm256_set1_epi64x(0xFFFFFFFF00000000); // all
    vmsk_lsb48 = _mm256_set1_epi64x(LSB48); // all 
    vmsk_seed = _mm256_set1_epi64x(0x7FFFFFFF); // only 31 LSB of seed considered

    int i;
    int offs = 0;
    int need = 1;
    int prime = 0;
    int prime_position = offs;
    VECTOR_INT vs;

    vs = _mm256_set_epi64x(s[3], s[2], s[1], s[0]);
    vs = _mm256_and_si256(vs, vmsk_seed); // s &= 0x7FFFFFFF

    getprime_32(need, &prime, offs);

    // NOTE: need to allow different value for streams
    *vprime = _mm256_set_epi64x(prime, prime, prime, prime);
    *vseed = _mm256_set_epi64x(INIT_SEED, INIT_SEED, INIT_SEED, INIT_SEED);

    vs = _mm256_slli_epi64(vs, 0x10); // seed << 16

    *vseed = _mm256_xor_si256(*vseed, vs); // seed ^= (s << 16)

    *vmult = _mm256_set_epi64x(mults_g[m[3]], mults_g[m[2]], mults_g[m[1]], mults_g[m[0]]); // NOTE: parameterize

    // Vector masks
    vmult_shf = _mm256_shuffle_epi32(*vmult, 0xB1); // shuffle multiplier 

    if (_mm256_testz_si256(*vprime, vmsk_lh64[0]) == 1) // all streams have prime = 0
        *vseed = _mm256_or_si256(*vseed, vmsk_lsb1[0]);
    else if (_mm256_testz_si256(*vprime, vmsk_lh64[1]) == 1) // first stream has prime = 0
        *vseed = _mm256_or_si256(*vseed, vmsk_lsb1[1]);
    else if (_mm256_testz_si256(*vprime, vmsk_lh64[2]) == 1) // second stream has prime = 0
        *vseed = _mm256_or_si256(*vseed, vmsk_lsb1[2]);
    else if (_mm256_testz_si256(*vprime, vmsk_lh64[3]) == 1) // third stream has prime = 0
        *vseed = _mm256_or_si256(*vseed, vmsk_lsb1[3]);
    else if (_mm256_testz_si256(*vprime, vmsk_lh64[4]) == 1) // fourth stream has prime = 0
        *vseed = _mm256_or_si256(*vseed, vmsk_lsb1[4]);

    for (i = 0; i < (LCGRUNUP * prime_position); ++i)
        get_vrn_dbl(vseed, *vmult, *vprime);
 
    return 0;
}


// SIMD multiply and new seed
VECTOR_INT get_vrn_int(VECTOR_INT *vseed, const VECTOR_INT vmultiplier, const VECTOR_INT vprime)
{
    VECTOR_INT st, vtmp, u, vrng;

    st = _mm256_mullo_epi32(*vseed, vmult_shf); // xl * yh, xh * yl
    //st = _mm_hadd_epi32(st, st); // s + t 
    vtmp = _mm256_slli_epi64(st, 0x20); // shift << 32 
    st = _mm256_add_epi64(st, vtmp); // s + t 
    st = _mm256_and_si256(st, vmsk_hi64); // (s + t) & 0xFFFFFFFF00000000
    u = _mm256_mul_epu32(*vseed, vmultiplier); // xl * yl
    *vseed = _mm256_add_epi64(u, st); // u + s + t 
    *vseed = _mm256_add_epi64(*vseed, vprime);    // seed += prime
    *vseed = _mm256_and_si256(*vseed, vmsk_lsb48); // seed &= LSB48
    vrng = _mm256_srli_epi64(*vseed, 0x11); // seed >> 17

    return vrng;
}


// SIMD multiply and new seed
VECTOR_DP get_vrn_dbl(VECTOR_INT *vseed, const VECTOR_INT vmultiplier, const VECTOR_INT vprime)
{
    VECTOR_INT st, vtmp, u;
    VECTOR_DP vseedd, vrng;
    const double val[4] = {RNG_LONG64_DBL, RNG_LONG64_DBL, RNG_LONG64_DBL, RNG_LONG64_DBL};
    unsigned long int seed[4] __attribute__ ((aligned(VECTOR_ALIGN)));
    double seedd[4];

    st = _mm256_mullo_epi32(*vseed, vmult_shf); // xl * yh, xh * yl
    //st = _mm_hadd_epi32(st, st); // s + t 
    vtmp = _mm256_slli_epi64(st, 0x20); // shift << 32 
    st = _mm256_add_epi64(st, vtmp); // s + t 
    st = _mm256_and_si256(st, vmsk_hi64); // (s + t) & 0xFFFFFFFF00000000
    u = _mm256_mul_epu32(*vseed, vmultiplier); // xl * yl
    *vseed = _mm256_add_epi64(u, st); // u + s + t 
    *vseed = _mm256_add_epi64(*vseed, vprime);    // seed += prime
    *vseed = _mm256_and_si256(*vseed, vmsk_lsb48); // seed &= LSB48
    vrng = _mm256_load_pd(val); 

    // NOTE: To convert from unsigned long seed to double, need to store and load as double
    _mm256_store_si256((VECTOR_INT *)seed, *vseed);
    seedd[0] = (double)seed[0];
    seedd[1] = (double)seed[1];
    seedd[2] = (double)seed[2];
    seedd[3] = (double)seed[3];
    vseedd =    _mm256_load_pd((double *)seedd);
    vrng = _mm256_mul_pd(vseedd, vrng); // seed * constant

    return vrng;
}


// SIMD multiply and new seed
VECTOR_SP get_vrn_flt(VECTOR_INT *vseed, const VECTOR_INT vmultiplier, const VECTOR_INT vprime)
{
    return _mm256_castps128_ps256(_mm256_cvtpd_ps(get_vrn_dbl(vseed, vmultiplier, vprime)));
}


#elif defined(USE_SSE)
// Global constants
static VECTOR_INT vmsk_lsb1[3];
static VECTOR_INT vmsk_lh64[3];
static VECTOR_INT vmsk_hi64;
static VECTOR_INT vmsk_lsb48;
static VECTOR_INT vmsk_seed;
static VECTOR_INT vmult_shf;

// Initialize SIMD RNG
int init_vrng(VECTOR_INT *vseed, VECTOR_INT *vmult, VECTOR_INT *vprime, int *s, int *m)
{
    // Vector masks
    vmsk_lsb1[0] = _mm_set1_epi64x(0x1); // all 
    vmsk_lsb1[1] = _mm_set_epi64x(0x0, 0x1); // first
    vmsk_lsb1[2] = _mm_set_epi64x(0x1, 0x0); // second
    vmsk_lh64[0] = _mm_set1_epi64x(0xFFFFFFFFFFFFFFFF); // all 
    vmsk_lh64[1] = _mm_set_epi64x(0x0, 0xFFFFFFFFFFFFFFFF); // first
    vmsk_lh64[2] = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0); // second
    vmsk_hi64 = _mm_set1_epi64x(0xFFFFFFFF00000000); // all
    vmsk_lsb48 = _mm_set1_epi64x(LSB48); // all
    vmsk_seed = _mm_set1_epi64x(0x7FFFFFFF); // only 31 LSB of seed considered

    int i;
    int offs = 0;
    int need = 1;
    int prime = 0;
    int prime_position = offs;
    VECTOR_INT vs;

    vs = _mm_set_epi64x(s[1], s[0]);
    vs = _mm_and_si128(vs, vmsk_seed); // s &= 0x7FFFFFFF

    getprime_32(need, &prime, offs);

    // NOTE: need to allow different value for streams
    *vprime = _mm_set_epi64x(prime, prime);
    *vseed = _mm_set_epi64x(INIT_SEED, INIT_SEED);

    vs = _mm_slli_epi64(vs, 0x10); // seed << 16
    *vseed = _mm_xor_si128(*vseed, vs); // seed ^= (s << 16)
    *vmult = _mm_set_epi64x(mults_g[m[1]], mults_g[m[0]]); // NOTE: parameterize

    // Vector masks
    vmult_shf = _mm_shuffle_epi32(*vmult, 0xB1); // shuffle multiplier 

#if defined(__SSE4_1__)
    if (_mm_test_all_zeros(*vprime, vmsk_lh64[0]) == 1) // all streams have prime = 0
        *vseed = _mm_or_si128(*vseed, vmsk_lsb1[0]);
    else if (_mm_test_all_zeros(*vprime, vmsk_lh64[1]) == 1) // first stream has prime = 0
        *vseed = _mm_or_si128(*vseed, vmsk_lsb1[1]);
    else if (_mm_test_all_zeros(*vprime, vmsk_lh64[2]) == 1) // second stream has prime = 0
        *vseed = _mm_or_si128(*vseed, vmsk_lsb1[2]);
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
VECTOR_INT get_vrn_int(VECTOR_INT *vseed, const VECTOR_INT vmult, const VECTOR_INT vprime)
{
    VECTOR_INT st, vtmp, u, vrng;

#if defined(__SSE4_1__)
    vmult_shf = _mm_shuffle_epi32(vmult, 0xB1); // shuffle multiplier 
    st = _mm_mullo_epi32(*vseed, vmult_shf); // xl * yh, xh * yl
#else
    printf("ERROR: require at least SSE4.1\n");
    return _mm_setzero_si128();
#endif

    //st = _mm_hadd_epi32(st, st); // s + t 
    vtmp = _mm_slli_epi64(st, 0x20); // shift << 32 
    st = _mm_add_epi64(st, vtmp); // s + t 
    st = _mm_and_si128(st, vmsk_hi64); // (s + t) & 0xFFFFFFFF00000000
    u = _mm_mul_epu32(*vseed, vmult); // xl * yl
    *vseed = _mm_add_epi64(u, st); // u + s + t 
    *vseed = _mm_add_epi64(*vseed, vprime);    // seed += prime
    *vseed = _mm_and_si128(*vseed, vmsk_lsb48); // seed &= LSB48
    vrng = _mm_srli_epi64(*vseed, 0x11); // seed >> 17

    return vrng;
}


// SIMD multiply and new seed
VECTOR_DP get_vrn_dbl(VECTOR_INT *vseed, const VECTOR_INT vmult, const VECTOR_INT vprime)
{
    VECTOR_INT st, vtmp, u;
    VECTOR_DP vseedd, vrng;
    const double val[2] = {RNG_LONG64_DBL, RNG_LONG64_DBL};
    unsigned long int seed[2] __attribute__ ((aligned(VECTOR_ALIGN)));
    double seedd[2];

#if defined(__SSE4_1__)
    vmult_shf = _mm_shuffle_epi32(vmult, 0xB1); // shuffle multiplier 
    st = _mm_mullo_epi32(*vseed, vmult_shf); // xl * yh, xh * yl
#else
    printf("ERROR: require at least SSE4.1\n");
    return _mm_setzero_pd();
#endif

    //st = _mm_hadd_epi32(st, st); // s + t 
    vtmp = _mm_slli_epi64(st, 0x20); // shift << 32 
    st = _mm_add_epi64(st, vtmp); // s + t 
    st = _mm_and_si128(st, vmsk_hi64); // (s + t) & 0xFFFFFFFF00000000
    u = _mm_mul_epu32(*vseed, vmult); // xl * yl
    *vseed = _mm_add_epi64(u, st); // u + s + t 
    *vseed = _mm_add_epi64(*vseed, vprime);    // seed += prime
    *vseed = _mm_and_si128(*vseed, vmsk_lsb48); // seed &= LSB48
    vrng = _mm_load_pd(val);
    _mm_store_si128((VECTOR_INT *)seed, *vseed);
    seedd[0] = (double)seed[0];
    seedd[1] = (double)seed[1];
    vseedd =    _mm_load_pd((double *)seedd);
    vrng = _mm_mul_pd(vseedd, vrng); // seed * constant

    return vrng;
}


// SIMD multiply and new seed
VECTOR_SP get_vrn_flt(VECTOR_INT *vseed, const VECTOR_INT vmult, const VECTOR_INT vprime)
{
    return _mm_cvtpd_ps(get_vrn_dbl(vseed, vmult, vprime));
}
#endif
#endif // SIMD_MODE

