#include <stdio.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <sys/time.h>
#include <stdlib.h>
 
#define SMETHOD 3  // 1 (invalid), 2 (valid), 3 (valid)
#define VMETHOD 3  // 1 (similar v4), 2 (invalid), 3 (best), 4 (valid)
#define DEBUG
//#define CHECK 

#ifdef DEBUG
#define RNG_LIM 1
#else
#define RNG_LIM 1000 
#endif

#define INTG unsigned int
#define INTGL unsigned long int
#define ISHIFT 32

#define SEED 199140073644480
#define MULT1 0x2875a2e7b175L	/* 44485709377909  */
#define PRIME 11863279
#define LSB48 0xffffffffffffL

#ifdef DEBUG
#define aprintf(...) printf(__VA_ARGS__)
#define vprint_128i(str, val) \
  do { \
    int vb = 4; \
    INTG vtmpi[vb] __attribute__ ((aligned(16))); \
    _mm_store_si128((__m128i *)&vtmpi, val); \
    printf(str " = "); \
    for (int vi = 0; vi < vb; ++vi) \
      printf("%u\t", (unsigned)vtmpi[vi]); \
    printf("\n"); \
  } while(0);
#define vprintl_128i(str, val) \
  do { \
    int vb = 2; \
    INTGL vtmpi[vb] __attribute__ ((aligned(16))); \
    _mm_store_si128((__m128i *)&vtmpi, val); \
    printf(str " = "); \
    for (int vi = 0; vi < vb; ++vi) \
      printf("%lu\t", (long unsigned)vtmpi[vi]); \
    printf("\n"); \
  } while(0);
#else
#define aprintf(...)
#define vprint_128i(str, val)
#define vprintl_128i(str, val)
#endif


int mult_seed(INTGL *, INTGL, INTG);
int mult_seed_simd(INTGL *, INTGL *, INTGL *);
int mult_seed_simd_v3(INTGL *, INTGL *, INTGL *);
void startTime(long long int *);
double stopTime(long long int *);

int main()
{
  // Timers
  long long int timers[2];
  double t1, t2;

#ifdef CHECK 
  // Checks
  INTGL *aseed[2];
  int *arng[2];

  for (int i = 0; i < 2; ++i) {
    aseed[i] = (INTGL *)malloc(RNG_LIM * sizeof(INTGL));
    arng[i] = (int *)malloc(RNG_LIM * sizeof(int));
  }
#endif    

  // Scalar
  INTGL seed = SEED;
  INTGL multiplier = MULT1;
  INTG prime = PRIME;
  int rng;

  startTime(timers);
  for (int i = 0; i < RNG_LIM; ++i) {
#ifdef SMETHOD
    rng = mult_seed(&seed, multiplier, prime);
#else
    seed *= multiplier; //mult_48_64(&seed,&multiplier,&seed);
    seed += prime;
    seed &= LSB48;
    rng = seed >> 17;
#endif
#ifdef CHECK 
    aseed[0][i] = seed;
    arng[0][i] = rng;
#endif    
  }
  t1 = stopTime(timers);
  printf("scalar = %d\t%lu\t%lu\t%u\n", rng, seed, multiplier, prime);
  printf("\n");


  // SIMD
  INTGL seeds[2] __attribute__ ((aligned(16))) = {SEED, SEED};
  INTGL multipliers[2] __attribute__ ((aligned(16))) = {MULT1, MULT1}; 
  INTGL primes[2] __attribute__ ((aligned(16))) = {PRIME, PRIME}; 
  int rng2;

  startTime(timers);
  for (int i = 0; i < RNG_LIM; ++i) {
#ifdef VMETHOD
#if VMETHOD == 3
    rng2 = mult_seed_simd_v3(seeds, multipliers, primes);
#else
    rng2 = mult_seed_simd(seeds, multipliers, primes);
#endif
#else
    seeds[0] *= multipliers[0]; //mult_48_64(&seed,&multiplier,&seed);
    seeds[0] += primes[0];
    seeds[0] &= LSB48;
    rng2 = seeds[0] >> 17;
#endif
#ifdef CHECK 
    aseed[1][i] = seeds[0];
    arng[1][i] = rng2;
#endif    
  }
  t2 = stopTime(timers);
  printf("vector = %d\t%lu\t%lu\t%u\n", rng2, seeds[0], multiplier, prime);
  printf("\n");

  // Speedup
  printf("scalar/speedup = %g\n", t1 / t2);

  // Validate run
  if (rng == rng2 && seed == seeds[0])
    printf("PASSED\n");
  else
    printf("FAILED\n");
  printf("\n");
   
#ifdef CHECK 
  // Checks
  for (int i = 0; i < RNG_LIM; ++i) {
    if (aseed[0][i] != aseed[1][i]) {
      printf("first seed error at loop %d\n", i + 1); 
      break;
    }
    if (arng[0][i] != arng[1][i]) {
      printf("first rng error at loop %d\n", i + 1); 
      break;
    }
  }

  for (int i = 0; i < 2; ++i) {
    free(aseed[i]);
    free(arng[i]);
  }
#endif    
 
  return 0;
}


int mult_seed_simd_v3(INTGL *seeds, INTGL *multipliers, INTGL *primes)
{
  int rngs[4] = {0, 0, 0, 0};

  __m128i vseed = _mm_load_si128((__m128i *)seeds); // x
  vprint_128i("xl, xh", vseed);

  const __m128i vmultiplier = _mm_load_si128((__m128i *)multipliers); // y
  vprint_128i("yl, yh", vmultiplier);

  const __m128i sy = _mm_shuffle_epi32(vmultiplier, 0xB1); // shuffle y 
  __m128i st = _mm_mullo_epi32(vseed, sy); // xl * yh, xh * yl
  vprint_128i("xl * yh, xh * yl", st);

  st = _mm_hadd_epi32(st, st); // s + t 
  vprint_128i("s + t", st);

  const __m128i vmsk = _mm_set_epi64x(0xFFFFFFFF00000000, 0xFFFFFFFF00000000);
  st = _mm_and_si128(st, vmsk); // (s + t) & 0xFFFFFFFF00000000
  vprint_128i("zu", st);

  const __m128i u = _mm_mul_epu32(vseed, vmultiplier); // xl * yl
  vprintl_128i("zl", u);

  vseed = _mm_add_epi64(u, st); // u + s + t 
  vprintl_128i("seed", vseed);

  const __m128i vprime = _mm_load_si128((__m128i *)primes);
  vseed = _mm_add_epi64(vseed, vprime);  // seed += prime
  vprintl_128i("seed+prime", vseed);

  const __m128i vmsk2 = _mm_set_epi64x(LSB48, LSB48);
  vseed = _mm_and_si128(vseed, vmsk2); // seed &= LSB48
  vprintl_128i("seed&LSB48", vseed);

  __m128i vrng = _mm_srli_epi64(vseed, 0x11); // seed >> 17

  _mm_store_si128((__m128i *)seeds, vseed);
  _mm_store_si128((__m128i *)rngs, vrng);

  return rngs[0];
}


int mult_seed_simd(INTGL *seeds, INTGL *multipliers, INTGL *primes)
{
  int rngs[4] = {0, 0, 0, 0};

  __m128i vseed = _mm_load_si128((__m128i *)seeds); // x
  vprint_128i("xl, xh", vseed);

  __m128i vmultiplier = _mm_load_si128((__m128i *)multipliers); // y
  vprint_128i("yl, yh", vmultiplier);

#if VMETHOD == 1
  __m128i sy = _mm_shuffle_epi32(vmultiplier, 0xB1); // shuffle y 
  __m128i st1 = _mm_mul_epu32(vseed, sy); // xl * yh
  vprint_128i("xl * yh", st1);

  __m128i sx = _mm_shuffle_epi32(vseed, 0xB1); // shuffle x 
  __m128i st2 = _mm_mul_epu32(sx, vmultiplier); // xh * yl
  vprint_128i("xh * yl", st2);

  __m128i st = _mm_add_epi64(st1, st2); // s + t 
  vprint_128i("s + t", st);

  st = _mm_slli_epi64(st, 0x20); // shift(s + t) 
  vprint_128i("zu", st);

  __m128i u = _mm_mul_epu32(vseed, vmultiplier); // xl * yl
  vprintl_128i("zl", u);

  vseed = _mm_add_epi64(u, st); // u + s + t 

#elif VMETHOD == 2
  __m128i sy = _mm_shuffle_epi32(vmultiplier, 0xB1); // shuffle y 
  __m128i st = _mm_mullo_epi32(vseed, sy); // xl * yh, xh * yl
  vprint_128i("xl * yh, xh * yl", st);

  st = _mm_hadd_epi32(st, st); // s + t 
  vprint_128i("s + t", st);

  __m128i u = _mm_mul_epu32(vseed, vmultiplier); // xl * yl
  vprint_128i("xl * yl", u);
  vprintl_128i("xl * yl", u);

  long int msk = 0xFFFFFFFF;
  __m128i vmsk = _mm_set_epi64x(msk, msk);
  __m128i z = _mm_and_si128(u, vmsk); // z & 0xFFFFFFFF
  vprint_128i("zl", z);
  
  st = _mm_add_epi64(u, st); // u + s + t
  vprint_128i("u + s + t", st);

  msk = 0xFFFFFFFF00000000;
  vmsk = _mm_set_epi64x(msk, msk);
  st = _mm_and_si128(st, vmsk); // ust & 0xFFFFFFFF00000000
  vprint_128i("zu", st);

  vseed = _mm_or_si128(z, st);

#elif VMETHOD == 3
  __m128i sy = _mm_shuffle_epi32(vmultiplier, 0xB1); // shuffle y 
  __m128i st = _mm_mullo_epi32(vseed, sy); // xl * yh, xh * yl
  vprint_128i("xl * yh, xh * yl", st);

  st = _mm_hadd_epi32(st, st); // s + t 
  vprint_128i("s + t", st);

  // NOTE: compare
  // 1. shift 
  //st = _mm_slli_epi64(st, 0x20); // shift(s + t) 
  // 2. mask
  __m128i vmsk = _mm_set_epi64x(0xFFFFFFFF00000000, 0xFFFFFFFF00000000);
  st = _mm_and_si128(st, vmsk); // (s + t) & 0xFFFFFFFF00000000
  vprint_128i("zu", st);

  __m128i u = _mm_mul_epu32(vseed, vmultiplier); // xl * yl
  vprintl_128i("zl", u);

  vseed = _mm_add_epi64(u, st); // u + s + t 

#elif VMETHOD == 4
  __m128i sy = _mm_shuffle_epi32(vmultiplier, 0xB1); // shuffle y 
  __m128i st1 = _mm_mul_epu32(vseed, sy); // xl * yh
  vprint_128i("xl * yh", st1);

  __m128i sx = _mm_shuffle_epi32(vseed, 0xB1); // shuffle x 
  __m128i st2 = _mm_mul_epu32(sx, vmultiplier); // xh * yl
  vprint_128i("xh * yl", st2);

  __m128i st = _mm_add_epi64(st1, st2); // s + t 
  vprint_128i("s + t", st);

  st = _mm_slli_epi64(st, 0x20); // shift(s + t) 
  vprint_128i("zu", st);

  __m128i u = _mm_mul_epu32(vseed, vmultiplier); // xl * yl
  vprintl_128i("zl", u);

  vseed = _mm_add_epi64(u, st); // u + s + t 
#endif

  vprintl_128i("seed", vseed);

  __m128i vprime = _mm_load_si128((__m128i *)primes);
  vseed = _mm_add_epi64(vseed, vprime);  // seed += prime
  vprintl_128i("seed+prime", vseed);

  __m128i vmsk2 = _mm_set_epi64x(LSB48, LSB48);
  vseed = _mm_and_si128(vseed, vmsk2); // seed &= LSB48
  vprintl_128i("seed&LSB48", vseed);

  __m128i vrng = _mm_srli_epi64(vseed, 0x11); // seed >> 17

  _mm_store_si128((__m128i *)seeds, vseed);
  _mm_store_si128((__m128i *)rngs, vrng);

  return rngs[0];
}


int mult_seed(INTGL *seed, INTGL y, INTG prime)
{
  // Algorithm
  // Get high/low parts 
  INTGL z1 = 0, x = *seed;
  INTG xh, xl;
  INTG yh, yl;
  //xl = x & 0xFFFFFFFF;
  xh = x >> ISHIFT;
  xl = x;
  aprintf("xl, xh = %u\t%u\n", xl, xh);

  //yl = y & 0xFFFFFFFF;
  yh = y >> ISHIFT;
  yl = y;
  aprintf("yl, yh = %u\t%u\n", yl, yh);

#if SMETHOD == 1
  // Method 1
  z1 = (xl * yl) + (xl * yh + xh * yl) * (INTGL)1 << ISHIFT; 

#elif SMETHOD == 2
  // Method 2
  INTG s = xl * yh;
  INTG t = xh * yl;
  aprintf("xl * yh, xh * yl = %u\t%u\n", s, t);

  INTG u = s + t;
  aprintf("s + t = %u\n", u);

  INTGL l = (INTGL)xl * yl;
  aprintf("xl * yl = %u\n", (INTG)l);
  aprintf("xl * yl = %lu\n", l);

  INTG lu = l >> ISHIFT;
  aprintf("lu = %lu\n", (INTGL)lu);

  INTGL zl = l & 0xFFFFFFFF;
  aprintf("zl = %lu\n", zl);
 
  INTGL zu = lu + u; 
  aprintf("zu = %lu\n", zu);
  zu = zu << ISHIFT;

  z1 = zu | zl;  

  //z1 = ((INTGL)(lu + s + t) << ISHIFT) | (l & 0xFFFFFFFF);  

#elif SMETHOD == 3
  // Method 3
  INTG s = xl * yh;
  INTG t = xh * yl;
  aprintf("xl * yh, xh * yl = %u\t%u\n", s, t);

  INTGL zu = s + t;
  aprintf("zu = %lu\n", zu);

  INTGL zl = (INTGL)xl * yl;
  aprintf("zl = %lu\n", zl);
  zu = zu << ISHIFT;

  z1 = zu + zl;  
#endif
  
  aprintf("seed = %lu\n", z1);

  z1 += prime;
  aprintf("seed+prime = %lu\n", z1);

  z1 &= LSB48;
  aprintf("seed&LSB48 = %lu\n", z1);

  *seed = z1;

  return z1 >> 17;
}


// Timing functions
void startTime(long long int *timers)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    timers[0] = tv.tv_sec;
    timers[1] = tv.tv_usec;
}


double stopTime(long long int *timers)
{
    double rtime;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    if (tv.tv_usec >= timers[1]) {
        timers[0] = tv.tv_sec - timers[0];
        timers[1] = tv.tv_usec - timers[1];
        rtime = (double)timers[0] + timers[1] / 1000000.0;
    }
    else {
        timers[0] = tv.tv_sec - timers[0] - 1;
        rtime = (double)timers[0] + 1.0 - (timers[1] - tv.tv_usec) / 1000000.0;
    }
    printf("Real time = %.16lf sec\n", rtime);

    return rtime;
}

