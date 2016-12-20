#include <stdio.h>
#include <stdlib.h>
#include "masprng2.h"
#include "utils.h"
#include "check2.h"


// Check errors with SPRNG data output
int check_errors(void)
{
    int i;  // iteration variables
    int rval;  // function return value

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

    int *primes = NULL;
    rval = posix_memalign((void **)&primes, SIMD_ALIGN, nstrms32 * sizeof(int));
    for (i = 0; i < nstrms32; ++i)
        primes[i] = 0;

    int *irngs = NULL;
    rval = posix_memalign((void **)&irngs, SIMD_ALIGN, nstrms32 * sizeof(int));
    for (i = 0; i < nstrms32; ++i)
        irngs[i] = 0;

    float *frngs = NULL;
    rval = posix_memalign((void **)&frngs, SIMD_ALIGN, nstrms32 * sizeof(float));
    for (i = 0; i < nstrms32; ++i)
        frngs[i] = 0;

    double *drngs = NULL;
    rval = posix_memalign((void **)&drngs, SIMD_ALIGN, nstrms64 * sizeof(double));
    for (i = 0; i < nstrms64; ++i)
        drngs[i] = 0;

    // LCG RNG object
    LCG rng;

    // Initialize RNG params
    for (i = 0; i < nstrms; ++i)
        rng.init_rng(iseeds[i], m[i]);

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

    // Integer
    int *irngs2 = NULL;
    rval = posix_memalign((void **)&irngs2, SIMD_ALIGN, nstrms32 * sizeof(int));
    for (i = 0; i < nstrms32; ++i)
        irngs2[i] = 0;

    // Float 
    float *frngs2 = NULL;
    rval = posix_memalign((void **)&frngs2, SIMD_ALIGN, nstrms32 * sizeof(float));
    for (i = 0; i < nstrms32; ++i)
        frngs2[i] = 0;

    // Double 
    double *drngs2 = NULL;
    rval = posix_memalign((void **)&drngs2, SIMD_ALIGN, nstrms64 * sizeof(double));
    for (i = 0; i < nstrms64; ++i)
        drngs2[i] = 0;

    SIMD_INT vseeds = simd_load(seeds2);
    SIMD_INT vmults = simd_load(mults2);
    SIMD_INT vprimes = simd_load(primes2);
    SIMD_INT ivrngs = simd_load(irngs2);
    SIMD_SP fvrngs = simd_load(frngs2);
    SIMD_DP dvrngs = simd_load(drngs2);

    // Initial RNG params 
    init_vrng(&vseeds, &vmults, &vprimes, iseeds, m);
#endif

    // Validate run
    const unsigned int fltmult = 1 << 20, dblmult = 1 << 30; // NOTE: from SPRNG LCG
    int rn = 0;

    // Integer generator
    int valid = 1;
    for (i = 0; i < 200; ++i) {
        rval = scanf("%d\n", &rn);
        irngs[0] = rng.get_rn_int();

#if defined(SIMD_MODE)
        ivrngs = get_vrn_int(&vseeds, vmults, vprimes);
        simd_store(seeds2, vseeds);
        simd_store(irngs2, ivrngs);

        if ((rn != irngs[0]) || (irngs[0] != irngs2[0])) {
            valid = 0;
            printf("%d\t%d\t%d\t%lu\n", rn, irngs[0], irngs2[0], seeds[0]);
        }
#else
        if (rn != irngs[0]) {
            valid = 0;
            printf("%d\t%d\t%lu\n", rn, irngs[0], rng.get_seed());
        }
#endif
    }

    if (valid > 0)
        printf("PASSED: Integer generator passed the reproducibility test.\n");
    else
        printf("FAILED: Integer generator does not reproduce correct stream.\n");
    printf("\n");


    // Float generator
    valid = 1;
    for (i = 0; i < 50; ++i) {
        rval = scanf("%d\n", &rn);
        frngs[0] = rng.get_rn_flt();

#if defined(SIMD_MODE)
        fvrngs = get_vrn_flt(&vseeds, vmults, vprimes);
        simd_store(seeds2, vseeds);
        simd_store(frngs2, fvrngs);

        const int rn1 = rn >> 11;
        const int rn2 = (int)(frngs[0] * fltmult);
        const int rn3 = (int)(frngs2[0] * fltmult);
        if ((abs(rn1 - rn2) > 1) || (rn2 != rn3)) {
            valid = 0;
            printf("%d\t%d\t%d\t%lu\n", rn1, rn2, rn3, seeds[0]);
        }
#else
        const int rn1 = rn >> 11;
        const int rn2 = (int)(frngs[0] * fltmult);
        if (abs(rn1 - rn2) > 1) {
            valid = 0;
            printf("%d\t%d\t%lu\n", rn1, rn2, rng.get_seed());
        }
#endif
    }

    if (valid > 0)
        printf("PASSED: Float generator passed the reproducibility test.\n");
    else
        printf("FAILED: Float generator does not reproduce correct stream.\n");
    printf("\n");


    // Double generator
    valid = 1;
    for (i = 0; i < 50; ++i) {
        rval = scanf("%d\n", &rn);
        drngs[0] = rng.get_rn_dbl();

#if defined(SIMD_MODE)
        dvrngs = get_vrn_dbl(&vseeds, vmults, vprimes);
        simd_store(seeds2, vseeds);
        simd_store(drngs2, dvrngs);

        int rn1 = rn >> 1;
        int rn2 = (int)(drngs[0] * dblmult);
        int rn3 = (int)(drngs2[0] * dblmult);
        if ((abs(rn1 - rn2) > 1) || (rn2 != rn3)) {
            valid = 0;
            printf("%d\t%d\t%d\t%lu\n", rn1, rn2, rn3, seeds[0]);
        }
#else
        int rn1 = rn >> 1;
        int rn2 = (int)(drngs[0] * dblmult);
        if (abs(rn1 - rn2) > 1) {
            valid = 0;
            printf("%d\t%d\t%lu\n", rn1, rn2, rng.get_seed());
        }
#endif
    }

    if (valid > 0)
        printf("PASSED: Double generator passed the reproducibility test.\n");
    else
        printf("FAILED: Double generator does not reproduce correct stream.\n");
    printf("\n");


#if defined(SIMD_MODE)
    // Deallocate memory
    free(seeds2);
    free(mults2);
    free(primes2);
    free(irngs2);
    free(frngs2);
    free(drngs2);
#endif
    
    // Deallocate memory
    free(iseeds);
    free(seeds);
    free(mults);
    free(primes);
    free(irngs);
    free(frngs);
    free(drngs);
 
    return 0;
}

