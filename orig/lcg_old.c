#include <stdio.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <sys/time.h>
#include <stdlib.h>
#include <omp.h>
 
//#define DEBUG
//#define CHECK 

#ifdef DEBUG
#define RNG_LIM 1 
#else
#define RNG_LIM 100
#endif

// OpenMP settings
#define SCHED_OMP dynamic   // static, dynamic, guided, runtime
#define CHUNK_OMP 8         // chunk size of scheduling option
#define NTHRD_OMP 4         // number of threads for OpenMP

#define SEED 199140073644480
#define PRIME 11863279

///////////////////////////
// Constants from lcg.cpp
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
#define MULT1 0x2875a2e7b175L	/* 44485709377909  */
#define MULT2 0x5deece66dL	/* 1575931494      */
#define MULT3 0x3eac44605265L	/* 68909602460261  */
#define MULT4 0x275b38eb4bbdL	/* 4327250451645   */
#define MULT5 0x1ee1429cc9f5L	/* 33952834046453  */
#define MULT6 0x739a9cb08605L	/* 127107890972165 */
#define MULT7 0x3228d7cc25f5L	/* 55151000561141  */

#define TWO_M24 5.96046447753906234e-8
#define TWO_M48 3.5527136788005008323e-15

/*
#ifdef LittleEndian
#define MSB 1
#else
#define MSB 0
#endif
#define LSB (1-MSB)
*/

#define LCGRUNUP 29

unsigned long int mults_g[] = {MULT1, MULT2, MULT3, MULT4, MULT5, MULT6, MULT7};
unsigned long int multiplier_g = 0;
///////////////////////////

#ifdef DEBUG
#define aprint(...) printf(__VA_ARGS__)
#define vprint_128i(str, val) \
  do { \
    int vb = 4; \
    unsigned int vtmpi[vb] __attribute__ ((aligned(16))); \
    _mm_store_si128((__m128i *)&vtmpi, val); \
    printf(str " = "); \
    for (int vi = 0; vi < vb; ++vi) \
      printf("%u\t", (unsigned int)vtmpi[vi]); \
    printf("\n"); \
  } while(0);
#define vprintl_128i(str, val) \
  do { \
    int vb = 2; \
    unsigned long int vtmpi[vb] __attribute__ ((aligned(16))); \
    _mm_store_si128((__m128i *)&vtmpi, val); \
    printf(str " = "); \
    for (int vi = 0; vi < vb; ++vi) \
      printf("%lu\t", (unsigned long int)vtmpi[vi]); \
    printf("\n"); \
  } while(0);
#else
#define aprint(...)
#define vprint_128i(str, val)
#define vprintl_128i(str, val)
#endif


void setOmpEnv();
void startTime(long long int *);
double stopTime(long long int *);


// Global constants
__m128i vmsk_lsb1[3];
__m128i vmsk_lh64[3];
__m128i vmsk_hi64;
__m128i vmsk_lsb48;
__m128i sy;
int rng_lim = RNG_LIM;


int main(int argc, char **argv)
{
  // Parse CLI args
  if (argc > 1)
    rng_lim = atoi(argv[1]); // get number of iterations

  // OpenMP
  setOmpEnv();

  // Timers
  long long int timers[2];
  double t1, t2;

#ifdef CHECK 
  // Checks
  unsigned long int *aseed[2];
  int *arng[2];

  for (int i = 0; i < 2; ++i) {
    aseed[i] = (unsigned long int *)malloc(rng_lim * sizeof(unsigned long int));
    arng[i] = (int *)malloc(rng_lim * sizeof(int));
  }
#endif    

  // Scalar
  unsigned long int seed = 985456376;  // NOTE: from main_lcg.cpp
  unsigned long int multiplier = 0;
  unsigned int prime = 0;
  int rng = 0;

  // Initialize RNG params
  init_rng(&seed, &multiplier, &prime);

  startTime(timers);
#ifdef _OPENMP
  #pragma omp parallel for default(shared) private(rng, seed) schedule(SCHED_OMP, CHUNK_OMP)
#endif
  for (int i = 0; i < rng_lim; ++i) {
    rng = mult_seed(&seed, multiplier, prime);
#ifdef CHECK 
    aseed[0][i] = seed;
    arng[0][i] = rng;
#endif    
  }
  t1 = stopTime(timers);
  printf("scalar = %d\t%lu\t%lu\t%u\n", rng, seed, multiplier, prime);
  printf("\n");


  // SIMD
  unsigned long int seeds[2] __attribute__ ((aligned(16))) = {985456376, 985456376};
  unsigned long int multipliers[2] __attribute__ ((aligned(16))) = {0, 0}; 
  unsigned int primes[4] __attribute__ ((aligned(4))) = {0, 0, 0, 0}; 
  int rngs[4] = {0, 0, 0, 0};
  __m128i vrng = _mm_setzero_si128();

  // Initial setup
  __m128i vseed[1] = {_mm_load_si128((__m128i *)seeds)};
  __m128i vmultiplier = _mm_load_si128((__m128i *)multipliers);
  __m128i vprime = _mm_load_si128((__m128i *)primes);

  // Vector masks
  vmsk_lsb1[0] = _mm_set_epi64x(0x1, 0x1); // both
  vmsk_lsb1[1] = _mm_set_epi64x(0x0, 0x1); // first only
  vmsk_lsb1[2] = _mm_set_epi64x(0x1, 0x0); // second only
  vmsk_lh64[0] = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF); // both
  vmsk_lh64[1] = _mm_set_epi64x(0x0, 0xFFFFFFFFFFFFFFFF); // first only
  vmsk_lh64[2] = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0); // second only
  vmsk_hi64 = _mm_set_epi64x(0xFFFFFFFF00000000, 0xFFFFFFFF00000000); // both high
  vmsk_lsb48 = _mm_set_epi64x(LSB48, LSB48); // both

  // Initialize RNG params
  init_rng_simd(vseed, &vmultiplier, &vprime);

  // Post-init
  sy = _mm_shuffle_epi32(vmultiplier, 0xB1); // shuffle multiplier 

  startTime(timers);
#ifdef _OPENMP
  #pragma omp parallel for default(shared) private(vrng, vseed) schedule(SCHED_OMP, CHUNK_OMP)
#endif
  for (int i = 0; i < rng_lim; ++i) {
    vrng = mult_seed_simd(vseed, vmultiplier, vprime);
#ifdef CHECK 
    _mm_store_si128((__m128i *)seeds, vseed[0]);
    _mm_store_si128((__m128i *)rngs, vrng);
    aseed[1][i] = seeds[0];
    arng[1][i] = rngs[0];
#endif    
  }
  t2 = stopTime(timers);

  // Final setup
  _mm_store_si128((__m128i *)seeds, vseed[0]);
  _mm_store_si128((__m128i *)rngs, vrng);

  _mm_store_si128((__m128i *)multipliers, vmultiplier);
  _mm_store_si128((__m128i *)primes, vprime);
  printf("vector = %d\t%lu\t%lu\t%u\n", rngs[0], seeds[0], multipliers[0], primes[0]);
  printf("vector = %d\t%lu\t%lu\t%u\n", rngs[2], seeds[1], multipliers[1], primes[2]);
  printf("\n");

  // Info
  printf("RNG nums = %d\n", rng_lim);

  // Speedup
  if (t2 > 0)
    printf("speedup = scalar/vector = %g\n", t1 / t2);
  else
    printf("invalid speedup = %g/%g\n", t1, t2);

  // Validate run
  if (rng == rngs[0] && seed == seeds[0])
    if (rng == rngs[2] && seed == seeds[1])
      printf("PASSED\n");
    else
      printf("FAILED\n");
  else
    printf("FAILED\n");
  printf("\n");
   
#ifdef CHECK 
  // Checks
  for (int i = 0; i < rng_lim; ++i) {
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


int init_rng(unsigned long int *seed, unsigned long int *mult, unsigned int *prime)
{
  unsigned long int s = *seed;

  //getprime_32(1, prime, gn);
  *prime = PRIME;  // NOTE: for debug
  aprint("prime = %u\n", *prime);

  *seed = INIT_SEED;
  aprint("seed = %lu\n", *seed);

  //*seed ^= (unsigned long int)s << 16;
  s = (unsigned long int)s << 16;
  aprint("s-shift = %lu\n", s);
  *seed ^= s;
  aprint("seed = %lu\n", *seed);

  *mult = mults_g[0]; // NOTE: parameterize
  aprint("mult = %lu\n", *mult);

  if (*prime == 0)
    *seed |= 1;

  // NOTE: paramterize the 0
  //for (int i = 0; i < LCGRUNUNP * 0; ++i)
    // get_rn_dbl();
 
  return 0;
}


int init_rng_simd(__m128i *vseed, __m128i *vmult, __m128i *vprime)
{
  __m128i s = *vseed;

  //getprime_32(1, vprime, gn);
  *vprime = _mm_set_epi64x(PRIME, PRIME);  // NOTE: for debug
  vprint_128i("prime", *vprime);

  *vseed = _mm_set_epi64x(INIT_SEED, INIT_SEED);
  vprintl_128i("seed", *vseed);

  s = _mm_slli_epi64(s, 0x10); // seed << 16
  vprintl_128i("s-shift", s);

  *vseed = _mm_xor_si128(*vseed, s); // seed ^= (s << 16)
  vprintl_128i("seed", *vseed);

  *vmult = _mm_set_epi64x(mults_g[0], mults_g[0]); // NOTE: parameterize
  vprintl_128i("mult", *vmult);

  if (_mm_test_all_zeros(*vprime, vmsk_lh64[0]) == 1) { // all streams have prime = 0
    *vseed = _mm_or_si128(*vseed, vmsk_lsb1[0]);
    vprintl_128i("both zero", *vseed);
  }
  else if (_mm_test_all_zeros(*vprime, vmsk_lh64[1]) == 1) { // lower (first) stream has prime = 0
    *vseed = _mm_or_si128(*vseed, vmsk_lsb1[1]);
    vprintl_128i("first zero", *vseed);
  }
  else if (_mm_test_all_zeros(*vprime, vmsk_lh64[2]) == 1) { // upper (second) stream has prime = 0
    *vseed = _mm_or_si128(*vseed, vmsk_lsb1[2]);
    vprintl_128i("second zero", *vseed);
  }

  // NOTE: paramterize the 0
  //for (int i = 0; i < LCGRUNUNP * 0; ++i)
    // get_rn_dbl();
 
  return 0;
}


__m128i mult_seed_simd(__m128i *vseed, const __m128i vmultiplier, const __m128i vprime)
{
  vprintl_128i("seed", *vseed);
  vprint_128i("xl, xh", *vseed);
  vprint_128i("yl, yh", vmultiplier);

  __m128i st = _mm_mullo_epi32(*vseed, sy); // xl * yh, xh * yl
  vprint_128i("xl * yh, xh * yl", st);

  //st = _mm_hadd_epi32(st, st); // s + t 
  const __m128i vtmp = _mm_slli_epi64(st, 0x20); // shift << 32 
  st = _mm_add_epi64(st, vtmp); // s + t 
  vprint_128i("vtmp", vtmp);
  vprint_128i("s + t", st);

  st = _mm_and_si128(st, vmsk_hi64); // (s + t) & 0xFFFFFFFF00000000
  vprint_128i("zu", st);

  const __m128i u = _mm_mul_epu32(*vseed, vmultiplier); // xl * yl
  vprintl_128i("zl", u);

  *vseed = _mm_add_epi64(u, st); // u + s + t 
  vprintl_128i("seed", *vseed);

  *vseed = _mm_add_epi64(*vseed, vprime);  // seed += prime
  vprintl_128i("seed+prime", *vseed);

  *vseed = _mm_and_si128(*vseed, vmsk_lsb48); // seed &= LSB48
  vprintl_128i("seed&LSB48", *vseed);

  __m128i vrng = _mm_srli_epi64(*vseed, 0x11); // seed >> 17
  vprint_128i("rng", vrng);

  return vrng;
}

int mult_seed(unsigned long int *seed, const unsigned long int multiplier, const unsigned int prime)
{
  *seed *= multiplier; //mult_48_64(&seed,&multiplier,&seed);
  *seed += prime;
  *seed &= LSB48;

  return *seed >> 17;
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


// Configure OpenMP environment
void setOmpEnv()
{
#ifdef _OPENMP
#ifndef OMP_NUM_THREADS
    omp_set_num_threads(NTHRD_OMP);
#endif

#ifndef KMP_AFFINITY
    putenv("KMP_AFFINITY=granularity=fine,scatter");
#endif
#endif
}
