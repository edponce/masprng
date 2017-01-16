#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "masprng.h"
#include "timers.h"
#include "utils.h"
#include "vutils.h"
#include "check.h"


#if defined(DEBUG)
#define RNG_LIM 1
#else
#define RNG_LIM 100
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
    if (!detectProcSIMD()) {
        printf("ERROR: current system does not supports requested vector extensions.\n");
        return 0;
    }
    printSysconf();

    int rng_lim = RNG_LIM;
	int retval = 0;
    if (argc > 1)
        rng_lim = atoi(argv[1]);

    if (rng_lim > 0)
        retval = main_gen(rng_lim);
    else
        retval = check_gen(RNG_TYPE_NUM);

	if (retval)
		printf("ERROR: program could not run successfully.\n");

    return 0;
}


int main_gen(int rng_lim)
{
    int i, j;

    long int timers[2];
    double t1;
    const int nstrms = SIMD_STREAMS_32;

    // Info/speedup
    printf("RNG runs = %d\n", rng_lim);
    printf("Number of streams = %d\n\n", RNG_ELEMS);

    // Initial seeds
    int iseeds[nstrms];
    for (i = 0; i < nstrms; ++i)
        iseeds[i] = 985456376 - i;

    // Initial multiplier indices 
    int m[nstrms];
    for (i = 0; i < nstrms; ++i)
        m[i] = i % 7;

    // Scalar
#if defined(DEBUG)
# if defined(LONG_SPRNG)
    unsigned long int seeds[nstrms];
    unsigned long int mults[nstrms];
# else
    int seeds[nstrms];
    int mults[nstrms];
#endif
    int primes[nstrms];
#endif

    // Integer/float/double
    RNG_TYPE rngs[RNG_ELEMS];

    // RNG object
    SPRNG *rng[RNG_ELEMS];
    for (i = 0; i < RNG_ELEMS; ++i) {
        rng[i] = selectType(RNG_TYPE_NUM);
        rng[i]->init_rng(0, 1, iseeds[i], m[i]);
		if (!rng[i])
			return -1;
    }

    // Run kernel
    startTime(timers);
    for (i = 0; i < rng_lim; ++i) {
        for (j = 0; j < RNG_ELEMS; ++j) {
            rngs[j] = rng[j]->get_rn();

#if defined(DEBUG)
            seeds[j] = rng[j]->get_seed();
            mults[j] = rng[j]->get_multiplier();
            primes[j] = rng[j]->get_prime();
#endif
        }
    }
    t1 = stopTime(timers);

    // Print results 
    printf("gen nums %d\n", rng[RNG_ELEMS-1]->get_ngens());
    printf("Scalar real time = %.16f sec\n", t1);
    for (i = 0; i < RNG_ELEMS; ++i)
#if defined(DEBUG)
# if defined(LONG_SPRNG)
        printf("scalar = " RNG_FMT "\t%lu\t%lu\t%d\n", rngs[i], seeds[i], mults[i], primes[i]);
# else
        printf("scalar = " RNG_FMT "\t%d\t%d\t%d\n", rngs[i], seeds[i], mults[i], primes[i]);
# endif
#else
        printf("scalar = " RNG_FMT "\n", rngs[i]);
#endif
    printf("\n");


    // SIMD
#if defined(SIMD_MODE)
    double t2;

# if defined(DEBUG)
#   if defined(LONG_SPRNG)
    unsigned long int seeds2[nstrms];
    unsigned long int mults2[nstrms];
#   else
    int seeds2[nstrms];
    int mults2[nstrms];
#   endif
    int primes2[nstrms];
# endif

    // Integer/float/double
    RNG_TYPE rngs2[nstrms];
    VRNG_TYPE vrngs;

    // RNG object
    VSPRNG *vrng = selectTypeSIMD(RNG_TYPE_NUM);
	if (!vrng)
		return -1;
    vrng->init_rng(0, 1, iseeds, m, nstrms);

    // Run kernel
    startTime(timers);
    for (i = 0; i < rng_lim; ++i)
        vrngs = vrng->get_rn();
    t2 = stopTime(timers);

    // Print results 
# if defined(DEBUG)
    simd_storeu(primes2, vrng->get_prime());
    simd_storeu(seeds2, vrng->get_seed());
    simd_storeu(mults2, vrng->get_multiplier());
#   if defined(LONG_SPRNG)
    simd_storeu(seeds2+SIMD_STREAMS_64, vrng->get_seed2());
    simd_storeu(mults2+SIMD_STREAMS_64, vrng->get_multiplier2());
#   endif
# endif
    simd_storeu(rngs2, vrngs);

    printf("gen nums %d\n", vrng->get_ngens());
    printf("Vector real time = %.16f sec\n", t2);
    for (i = 0; i < RNG_ELEMS; ++i) 
# if defined(DEBUG)
#   if defined(LONG_SPRNG)
        printf("vector = " RNG_FMT "\t%lu\t%lu\t%d\n", rngs2[i], seeds2[i], mults2[i], primes2[i]);
#   else
        printf("vector = " RNG_FMT "\t%d\t%d\t%d\n", rngs2[i], seeds2[i], mults2[i], primes2[i]);
#   endif
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
    for (i = 0; i < RNG_ELEMS; ++i) { 
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

    // Clean SPRNG objects
    delete vrng;
#endif // SIMD_MODE

    // Clean SPRNG objects
    for (i = 0; i < RNG_ELEMS; ++i)
        delete rng[i];

    return 0;
}

