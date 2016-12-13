#include <stdio.h>
#include <stdlib.h>
#include "masprng.h"
#include "utils.h"
#include "check.h"


// Check errors with SPRNG data output
int check_errors(void)
{
    int i;  // iteration variables
    int rval;  // function return value

    const int nstrms = NSTRMS;
    const int nstrms32 = 2 * nstrms;
    const int nstrms64 = nstrms;

    // Initial seeds
    int *iseeds = NULL;
    rval = posix_memalign((void **)&iseeds, VECTOR_ALIGN, nstrms * sizeof(int));
    for (i = 0; i < nstrms; ++i)
        iseeds[i] = 985456376 - i;

    // Initial multiplier indices 
    int *m = NULL;
    rval = posix_memalign((void **)&m, VECTOR_ALIGN, nstrms * sizeof(int));
    for (i = 0; i < nstrms; ++i)
        m[i] = 0;

    // Scalar
    unsigned long int *seeds = NULL;
    rval = posix_memalign((void **)&seeds, VECTOR_ALIGN, nstrms64 * sizeof(unsigned long int));
    for (i = 0; i < nstrms64; ++i)
        seeds[i] = 0;

    unsigned long int *mults = NULL;
    rval = posix_memalign((void **)&mults, VECTOR_ALIGN, nstrms64 * sizeof(unsigned long int));
    for (i = 0; i < nstrms64; ++i)
        mults[i] = 0;

    unsigned int *primes = NULL;
    rval = posix_memalign((void **)&primes, VECTOR_ALIGN, nstrms32 * sizeof(unsigned int));
    for (i = 0; i < nstrms32; ++i)
        primes[i] = 0;

    int *irngs = NULL;
    rval = posix_memalign((void **)&irngs, VECTOR_ALIGN, nstrms32 * sizeof(int));
    for (i = 0; i < nstrms32; ++i)
        irngs[i] = 0;

    float *frngs = NULL;
    rval = posix_memalign((void **)&frngs, VECTOR_ALIGN, nstrms32 * sizeof(float));
    for (i = 0; i < nstrms32; ++i)
        frngs[i] = 0;

    double *drngs = NULL;
    rval = posix_memalign((void **)&drngs, VECTOR_ALIGN, nstrms64 * sizeof(double));
    for (i = 0; i < nstrms64; ++i)
        drngs[i] = 0;

    // Initialize RNG params
    for (i = 0; i < nstrms; ++i)
        init_rng(&seeds[i], &mults[i], &primes[i], iseeds[i], m[i]);

#if defined(SIMD_MODE)
    // SIMD
    unsigned long int *seeds2 = NULL;
    rval = posix_memalign((void **)&seeds2, VECTOR_ALIGN, nstrms * sizeof(unsigned long int));
    for (i = 0; i < nstrms; ++i)
        seeds2[i] = 0;

    unsigned long int *mults2 = NULL;
    rval = posix_memalign((void **)&mults2, VECTOR_ALIGN, nstrms64 * sizeof(unsigned long int));
    for (i = 0; i < nstrms64; ++i)
        mults2[i] = 0;

    unsigned int *primes2 = NULL;
    rval = posix_memalign((void **)&primes2, VECTOR_ALIGN, nstrms32 * sizeof(unsigned int));
    for (i = 0; i < nstrms32; ++i)
        primes2[i] = 0;

    // Integer
    int *irngs2 = NULL;
    rval = posix_memalign((void **)&irngs2, VECTOR_ALIGN, nstrms32 * sizeof(int));
    for (i = 0; i < nstrms32; ++i)
        irngs2[i] = 0;

    // Float 
    float *frngs2 = NULL;
    rval = posix_memalign((void **)&frngs2, VECTOR_ALIGN, nstrms32 * sizeof(float));
    for (i = 0; i < nstrms32; ++i)
        frngs2[i] = 0;

    // Double 
    double *drngs2 = NULL;
    rval = posix_memalign((void **)&drngs2, VECTOR_ALIGN, nstrms64 * sizeof(double));
    for (i = 0; i < nstrms64; ++i)
        drngs2[i] = 0;

    VECTOR_INT vseeds; rval = vload(&vseeds, seeds2);
    VECTOR_INT vmults; rval = vload(&vmults, mults2);
    VECTOR_INT vprimes; rval = vload(&vprimes, primes2);
    VECTOR_INT ivrngs; rval = vload(&ivrngs, irngs2);
    VECTOR_SP fvrngs; rval = vload(&fvrngs, frngs2);
    VECTOR_DP dvrngs; rval = vload(&dvrngs, drngs2);

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
        irngs[0] = get_rn_int(&seeds[0], mults[0], primes[0]);

#if defined(SIMD_MODE)
        ivrngs = get_vrn_int(&vseeds, vmults, vprimes);
        vstore(seeds2, vseeds);
        vstore(irngs2, ivrngs);

        if ((rn != irngs[0]) || (irngs[0] != irngs2[0])) {
            valid = 0;
            printf("%d\t%d\t%d\t%lu\n", rn, irngs[0], irngs2[0], seeds[0]);
        }
#else
        if (rn != irngs[0]) {
            valid = 0;
            printf("%d\t%d\t%lu\n", rn, irngs[0], seeds[0]);
        }
#endif
    }

#if defined(SIMD_MODE)
    if (valid > 0)
        printf("PASSED: Integer generator passed the reproducibility test.\n");
    else
        printf("FAILED: Integer generator does not reproduce correct stream.\n");
    printf("\n");
#endif


    // Float generator
    valid = 1;
    for (i = 0; i < 50; ++i) {
        rval = scanf("%d\n", &rn);
        frngs[0] = get_rn_flt(&seeds[0], mults[0], primes[0]);

#if defined(SIMD_MODE)
        fvrngs = get_vrn_flt(&vseeds, vmults, vprimes);
        vstore(seeds2, vseeds);
        vstore(frngs2, fvrngs);

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
            printf("%d\t%d\t%lu\n", rn1, rn2, seeds[0]);
        }
#endif
    }

#if defined(SIMD_MODE)
    if (valid > 0)
        printf("PASSED: Float generator passed the reproducibility test.\n");
    else
        printf("FAILED: Float generator does not reproduce correct stream.\n");
    printf("\n");
#endif


    // Double generator
    valid = 1;
    for (i = 0; i < 50; ++i) {
        rval = scanf("%d\n", &rn);
        drngs[0] = get_rn_dbl(&seeds[0], mults[0], primes[0]);

#if defined(SIMD_MODE)
        dvrngs = get_vrn_dbl(&vseeds, vmults, vprimes);
        vstore(seeds2, vseeds);
        vstore(drngs2, dvrngs);

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
            printf("%d\t%d\t%lu\n", rn1, rn2, seeds[0]);
        }
#endif
    }

#if defined(SIMD_MODE)
    if (valid > 0)
        printf("PASSED: Double generator passed the reproducibility test.\n");
    else
        printf("FAILED: Double generator does not reproduce correct stream.\n");
    printf("\n");
#endif

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

