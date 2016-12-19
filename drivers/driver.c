#include <stdio.h>
#include <stdlib.h>
#include "masprng.h"
#include "timers.h"
#include "utils.h"
//#include "check.h"


// Run settings
#if defined(DEBUG)
#define RNG_LIM 1 
#else
#define RNG_LIM 100
#endif


// Control type of test
#define TEST 0

#if TEST == 0
#define RNG_TYPE_STR "Integer"
#define RNG_TYPE int 
#define get_rn(...) get_rn_int(__VA_ARGS__)
#define VRNG_TYPE SIMD_INT 
#define get_vrn(...) get_vrn_int(__VA_ARGS__)
#define RNG_FMT "%d"
#define RNG_ELEMS nstrms32
#define RNG_SHIFT 2
#elif TEST == 1
#define RNG_TYPE_STR "Float"
#define RNG_TYPE float 
#define get_rn(...) get_rn_flt(__VA_ARGS__)
#define VRNG_TYPE SIMD_SP 
#define get_vrn(...) get_vrn_flt(__VA_ARGS__)
#define RNG_FMT "%f"
#define RNG_ELEMS nstrms32
#define RNG_SHIFT 1
#else 
#define RNG_TYPE_STR "Double"
#define RNG_TYPE double
#define get_rn(...) get_rn_dbl(__VA_ARGS__)
#define VRNG_TYPE SIMD_DP 
#define get_vrn(...) get_vrn_dbl(__VA_ARGS__)
#define RNG_FMT "%f"
#define RNG_ELEMS nstrms64
#define RNG_SHIFT 1
#endif


int run(int);


// Driver
int main(int argc, char **argv)
{
    // Parse CLI args
    int rng_lim = RNG_LIM;

    // Print system info
    printSysconf();
    printSIMDconf();
    printf("\n");

    if (argc > 1)
        rng_lim = atoi(argv[1]); // get number of iterations

    if (rng_lim > 0)
        run(rng_lim);
 //   else
 //       check_errors();

    return 0;
}


int run(int rng_lim)
{
    int i, j; // iteration variables
    int rval; // function return value

    // Configure OpenMP environment
    setOmpEnv(NULL);

    // Timers
    long long int timers[2];
    double t1, t2;

    const int nstrms = SIMD_NUM_STREAMS;
    const int nstrms32 = 2 * nstrms;
    const int nstrms64 = nstrms;

    // Initial seeds
    int *iseeds = NULL;
    rval = posix_memalign((void **)&iseeds, SIMD_ALIGN, nstrms * sizeof(int));
    for (i = 0; i < nstrms; ++i)
            iseeds[i] = 985456376 - i;

    // Initial multiplier indices 
    int *m = NULL;
    rval = posix_memalign((void **)&m, SIMD_ALIGN, nstrms * sizeof(int));
    for (i = 0; i < nstrms; ++i)
        m[i] = 0;

    // Scalar
    unsigned long int *seeds = NULL;
    rval = posix_memalign((void **)&seeds, SIMD_ALIGN, nstrms64 * sizeof(unsigned long int));
    for (i = 0; i < nstrms64; ++i)
        seeds[i] = 0;

    unsigned long int *mults = NULL;
    rval = posix_memalign((void **)&mults, SIMD_ALIGN, nstrms64 * sizeof(unsigned long int));
    for (i = 0; i < nstrms64; ++i)
        mults[i] = 0;

    unsigned int *primes = NULL;
    rval = posix_memalign((void **)&primes, SIMD_ALIGN, nstrms32 * sizeof(unsigned int));
    for (i = 0; i < nstrms32; ++i)
        primes[i] = 0;

    RNG_TYPE *rngs = NULL;
    rval = posix_memalign((void **)&rngs, SIMD_ALIGN, RNG_ELEMS * sizeof(RNG_TYPE));
    for (i = 0; i < RNG_ELEMS; ++i)
        rngs[i] = 0;

    // Initialize RNG params
    for (i = 0; i < nstrms; ++i)
        init_rng(&seeds[i], &mults[i], &primes[i], iseeds[i], m[i]);

    // Run kernel
    startTime(timers);
    #pragma omp parallel for ordered default(shared) firstprivate(rngs) lastprivate(rngs) schedule(SCHED_OMP)
    for (i = 0; i < rng_lim; ++i)
        #pragma omp ordered
        for (j = 0; j < nstrms; ++j)
            rngs[j] = get_rn(&seeds[j], mults[j], primes[j]);
    t1 = stopTime(timers);

    // Print results 
    printf("Real time = %.16f sec\n", t1);
    for (i = 0; i < nstrms; ++i)
        printf("scalar = " RNG_FMT "\t%lu\t%lu\t%u\n", rngs[i], seeds[i], mults[i], primes[i]);
    printf("\n");


#if defined(SIMD_MODE)
    // SIMD
    unsigned long int *seeds2 = NULL;
    rval = posix_memalign((void **)&seeds2, SIMD_ALIGN, nstrms * sizeof(unsigned long int));
    for (i = 0; i < nstrms; ++i)
        seeds2[i] = 0;

    unsigned long int *mults2 = NULL;
    rval = posix_memalign((void **)&mults2, SIMD_ALIGN, nstrms64 * sizeof(unsigned long int));
    for (i = 0; i < nstrms64; ++i)
        mults2[i] = 0;

    unsigned int *primes2 = NULL;
    rval = posix_memalign((void **)&primes2, SIMD_ALIGN, nstrms32 * sizeof(unsigned int));
    for (i = 0; i < nstrms32; ++i)
        primes2[i] = 0;

    RNG_TYPE *rngs2 = NULL;
    rval = posix_memalign((void **)&rngs2, SIMD_ALIGN, RNG_ELEMS * sizeof(RNG_TYPE));
    for (i = 0; i < RNG_ELEMS; ++i)
        rngs2[i] = 0;

    SIMD_INT vseeds = simd_load(seeds2);
    SIMD_INT vmults = simd_load(mults2);
    SIMD_INT vprimes = simd_load(primes2);
    VRNG_TYPE vrngs = simd_load(rngs2);

    // Initial RNG params 
    init_vrng(&vseeds, &vmults, &vprimes, iseeds, m);

    // Run kernel
    startTime(timers);
    #pragma omp parallel for ordered default(shared) firstprivate(vrngs) lastprivate(vrngs) schedule(SCHED_OMP)
    for (i = 0; i < rng_lim; ++i)
        #pragma omp ordered
        vrngs = get_vrn(&vseeds, vmults, vprimes);
    t2 = stopTime(timers);

    // Print results 
    simd_store(seeds2, vseeds);
    simd_store(mults2, vmults);
    simd_store(primes2, vprimes);
    simd_store(rngs2, vrngs);

    printf("Real time = %.16f sec\n", t2);
    for (i = 0; i < nstrms; ++i)
        printf("vector = " RNG_FMT "\t%lu\t%lu\t%u\n", rngs2[i*RNG_SHIFT], seeds2[i], mults2[i], primes2[i*2]);
    printf("\n");

    // Info
    printf("RNG runs = %d\n", rng_lim);
    printf("Number of streams = %d\n", nstrms);

    // Speedup
    if (t2 > 0)
        printf("speedup = scalar/vector = %g\n", t1 / t2);
    else
        printf("invalid speedup = %g/%g\n", t1, t2);

    // Validate run
    int valid = 1;
    for (i = 0; i < nstrms; ++i) { 
        if (rngs[i] != rngs2[i*RNG_SHIFT] || seeds[i] != seeds2[i]) {
            valid = 0;
            break;
        }
    }

    if (valid > 0)
        printf("PASSED: " RNG_TYPE_STR " generator passed the reproducibility test.\n");
    else
        printf("FAILED: " RNG_TYPE_STR " generator does not reproduce correct stream.\n");
    printf("\n");

    // Deallocate memory
    free(seeds2);
    free(mults2);
    free(primes2);
    free(rngs2);
#endif
    
    // Deallocate memory
    free(iseeds);
    free(seeds);
    free(mults);
    free(primes);
    free(rngs);
 
    return 0;
}

