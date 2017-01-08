#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include "masprng.h"
#include "timers.h"
#include "utils.h"
#include "check.h"


#if defined(DEBUG)
#define RNG_LIM 1
#else
#define RNG_LIM 100
#endif


#if !defined(SIMD_MODE)
const int SIMD_WIDTH_BYTES = 8;
const int SIMD_STREAMS_32 = 1;
const int SIMD_STREAMS_64 = 1;
#endif


// Control type of test
#define RNG_TYPE_NUM SPRNG_LCG
#define TEST 1


#if TEST == 0
#define RNG_TYPE_STR "Integer"
#define RNG_TYPE int 
#define get_rn() get_rn_int()
#define VRNG_TYPE SIMD_INT 
#define RNG_FMT "%d"
#define RNG_ELEMS SIMD_STREAMS_32 
#define RNG_NEQ(a,b) (a != b)
#elif TEST == 1
#define RNG_TYPE_STR "Float"
#define RNG_TYPE float 
#define get_rn() get_rn_flt()
#define VRNG_TYPE SIMD_FLT 
#define RNG_FMT "%f"
#define RNG_ELEMS SIMD_STREAMS_32
#define RNG_NEQ(a,b) (fabs(a-b) > FLT_EPSILON)
#else 
#define RNG_TYPE_STR "Double"
#define RNG_TYPE double
#define get_rn() get_rn_dbl()
#define VRNG_TYPE SIMD_DBL 
#define RNG_FMT "%f"
#define RNG_ELEMS SIMD_STREAMS_64 
#define RNG_NEQ(a,b) (fabs(a-b) > DBL_EPSILON)
#endif


int main_gen(int);


int main(int argc, char **argv)
{
    printf("\n");
    printSysconf();
    printSIMDconf();
    printf("\n");

    int rng_lim = RNG_LIM;

    if (argc > 1)
        rng_lim = atoi(argv[1]);

    if (rng_lim > 0)
        main_gen(rng_lim);
    else
        check_gen(RNG_TYPE_NUM);

    return 0;
}


int main_gen(int rng_lim)
{
    int i, j;
    int rval __attribute__((unused));

    long int timers[2];
    double t1;
    const int nstrms = RNG_ELEMS;

    // Info/speedup
    printf("RNG runs = %d\n", rng_lim);
    printf("Number of streams = %d\n\n", nstrms);

    // Initial seeds
    int *iseeds = NULL;
    rval = posix_memalign((void **)&iseeds, SIMD_WIDTH_BYTES, nstrms * sizeof(int));
    for (i = 0; i < nstrms; ++i)
            iseeds[i] = 985456376 - i;

    // Initial multiplier indices 
    int *m = NULL;
    rval = posix_memalign((void **)&m, SIMD_WIDTH_BYTES, nstrms * sizeof(int));
    for (i = 0; i < nstrms; ++i)
        m[i] = i;

    // Scalar
#if defined(DEBUG)
    unsigned long int *seeds = NULL;
    rval = posix_memalign((void **)&seeds, SIMD_WIDTH_BYTES, nstrms * sizeof(unsigned long int));

    unsigned long int *mults = NULL;
    rval = posix_memalign((void **)&mults, SIMD_WIDTH_BYTES, nstrms * sizeof(unsigned long int));

    int *primes = NULL;
    rval = posix_memalign((void **)&primes, SIMD_WIDTH_BYTES, SIMD_STREAMS_32 * sizeof(int));
#endif

    // Integer/float/double
    RNG_TYPE *rngs = NULL;
    rval = posix_memalign((void **)&rngs, SIMD_WIDTH_BYTES, RNG_ELEMS * sizeof(RNG_TYPE));

    // RNG object
    SPRNG *rng[nstrms];
    for (i = 0; i < nstrms; ++i) {
        rng[i] = selectType(RNG_TYPE_NUM);
        rng[i]->init_rng(0, 1, iseeds[i], m[i]);
    }

    // Run kernel
    startTime(timers);
    for (i = 0; i < rng_lim; ++i) {
        for (j = 0; j < nstrms; ++j) {
            rngs[j] = rng[j]->get_rn();

#if defined(DEBUG)
            seeds[j] = rng[j]->get_seed();
            primes[j] = rng[j]->get_prime();
            mults[j] = rng[j]->get_multiplier();
#endif
        }
    }
    t1 = stopTime(timers);

    // Print results 
    printf("gen nums %lu\n", rng[nstrms-1]->get_ngens());
    printf("Scalar real time = %.16f sec\n", t1);
    for (i = 0; i < nstrms; ++i)
#if defined(DEBUG)
        printf("scalar = " RNG_FMT "\t%lu\t%lu\t%u\n", rngs[i], seeds[i], mults[i], primes[i]);
#else
        printf("scalar = " RNG_FMT "\n", rngs[i]);
#endif
    printf("\n");


    // SIMD
#if defined(SIMD_MODE)
    double t2;

# if defined(DEBUG)
    unsigned long int *seeds2 = NULL;
    rval = posix_memalign((void **)&seeds2, SIMD_WIDTH_BYTES, nstrms * sizeof(unsigned long int));

    unsigned long int *mults2 = NULL;
    rval = posix_memalign((void **)&mults2, SIMD_WIDTH_BYTES, nstrms * sizeof(unsigned long int));

    unsigned int *primes2 = NULL;
    rval = posix_memalign((void **)&primes2, SIMD_WIDTH_BYTES, SIMD_STREAMS_32 * sizeof(unsigned int));
# endif

    // Integer/float/double
    RNG_TYPE *rngs2 = NULL;
    rval = posix_memalign((void **)&rngs2, SIMD_WIDTH_BYTES, RNG_ELEMS * sizeof(RNG_TYPE));

    VRNG_TYPE vrngs;

    // RNG object
    VSPRNG *vrng = selectTypeSIMD(RNG_TYPE_NUM);
    vrng->init_rng(0, 1, iseeds, m);

    // Run kernel
    startTime(timers);
    for (i = 0; i < rng_lim; ++i)
        vrngs = vrng->get_rn();
    t2 = stopTime(timers);

    // Print results 
# if defined(DEBUG)
    simd_store(seeds2, vrng->get_seed());
    simd_store(mults2, vrng->get_multiplier());
    simd_store(primes2, vrng->get_prime());
# endif
    simd_store(rngs2, vrngs);

    printf("gen nums %lu\n", vrng->get_ngens());
    printf("Vector real time = %.16f sec\n", t2);
    for (i = 0; i < nstrms; ++i)
# if defined(DEBUG)
        printf("vector = " RNG_FMT "\t%lu\t%lu\t%u\n", rngs2[i], seeds2[i], mults2[i], primes2[i*2]);
# else
        printf("vector = " RNG_FMT "\n", rngs2[i]);
# endif
    printf("\n");

    if (t2 > 0)
        printf("speedup = scalar/vector = %g\n", t1 / t2);
    else
        printf("invalid speedup = %g/%g\n", t1, t2);

    // Validate run
    int valid = 1;
    for (i = 0; i < nstrms; ++i) { 
# if defined(DEBUG)
        if (RNG_NEQ(rngs[i], rngs2[i]) || RNG_NEQ(seeds[i], seeds2[i])) {
# else
        if (RNG_NEQ(rngs[i], rngs2[i])) {
# endif
            valid = 0;
            break;
        }
    }

    if (valid > 0)
        printf("PASSED: " RNG_TYPE_STR " generator passed the reproducibility test.\n");
    else
        printf("FAILED: " RNG_TYPE_STR " generator does not reproduce correct stream.\n");
    printf("\n");

# if defined(DEBUG)
    free(seeds2);
    free(mults2);
    free(primes2);
# endif
    free(rngs2);
    delete vrng;
#endif // SIMD_MODE

    free(iseeds);
    free(m);
#if defined(DEBUG)
    free(seeds);
    free(mults);
    free(primes);
#endif
    free(rngs);
    for (i = 0; i < nstrms; ++i)
        delete rng[i];

    return 0;
}

