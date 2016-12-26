#include <stdio.h>
#include <stdlib.h>
#include "masprng2.h"
#include "utils.h"
#include "check2.h"


// Check errors with SPRNG data output
int check_errors(void)
{
    int i, j;  // iteration variables
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
    // Integer
    int *irngs = NULL;
    rval = posix_memalign((void **)&irngs, SIMD_ALIGN, nstrms32 * sizeof(int));

    // Float 
    float *frngs = NULL;
    rval = posix_memalign((void **)&frngs, SIMD_ALIGN, nstrms32 * sizeof(float));

    // Double 
    double *drngs = NULL;
    rval = posix_memalign((void **)&drngs, SIMD_ALIGN, nstrms64 * sizeof(double));

    // LCG RNG object
    LCG rng;

    // Initialize RNG params
    for (i = 0; i < nstrms; ++i)
        rng.init_rng(iseeds[i], m[i]);

#if defined(SIMD_MODE)
    // SIMD
    // Integer
    int *irngs2 = NULL;
    rval = posix_memalign((void **)&irngs2, SIMD_ALIGN, nstrms32 * sizeof(int));

    // Float 
    float *frngs2 = NULL;
    rval = posix_memalign((void **)&frngs2, SIMD_ALIGN, nstrms32 * sizeof(float));

    // Double 
    double *drngs2 = NULL;
    rval = posix_memalign((void **)&drngs2, SIMD_ALIGN, nstrms64 * sizeof(double));

    SIMD_INT ivrngs;
    SIMD_SP fvrngs;
    SIMD_DP dvrngs;

    // Initial RNG params 
    rng.init_vrng(iseeds, m);
#endif

    // Validate run
    const unsigned int fltmult = 1 << 20, dblmult = 1 << 30; // NOTE: from SPRNG LCG
    int rn = 0;
    int k; // index for scalar array

    // Integer generator
    int valid = 1;
    for (i = 0; i < 200; ++i) {
        rval = scanf("%d\n", &rn);
        k = i % nstrms;
        irngs[k] = rng.get_rn_int();

        if (rn != irngs[k]) {
            valid = 0;
            printf("File,scalar\t%d\t%d\n", rn, irngs[k]);
        }

#if defined(SIMD_MODE)
        if ((k+1) == nstrms) {
            ivrngs = rng.get_vrn_int();
            simd_store(irngs2, ivrngs);

            for (j = 0; j < nstrms; ++j) {
                if (irngs[j] != irngs2[j]) {
                    valid = 0;
                    printf("Scalar,vector\t%d\t%d\n", irngs[j], irngs2[j]);
                }
            }
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
        k = i % nstrms;
        frngs[k] = rng.get_rn_flt();

        int rn1 = rn >> 11;
        int rn2 = (int)(frngs[k] * fltmult);
        if (abs(rn1 - rn2) > 1) {
            valid = 0;
            printf("File,scalar\t%d\t%d\n", rn1, rn2);
        }

#if defined(SIMD_MODE)
        if ((k+1) == nstrms) {
            fvrngs = rng.get_vrn_flt();
            simd_store(frngs2, fvrngs);

            int rn3;
            for (j = 0; j < nstrms; ++j) {
                rn2 = (int)(frngs[j] * fltmult);
                rn3 = (int)(frngs2[j] * fltmult);
                if (rn2 != rn3) {
                    valid = 0;
                    printf("Scalar,vector\t%d\t%d\n", rn2, rn3);
                }
            }
        }
#endif
    }

    if (valid > 0)
        printf("PASSED: Float generator passed the reproducibility test.\n");
    else
        printf("FAILED: Float generator does not reproduce correct stream.\n");
    printf("\n");

/*
    // Double generator
    valid = 1;
    for (i = 0; i < 50; ++i) {
        rval = scanf("%d\n", &rn);
        drngs[0] = rng.get_rn_dbl();

#if defined(SIMD_MODE)
        dvrngs = rng.get_vrn_dbl();
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
*/


#if defined(SIMD_MODE)
    // Deallocate memory
    free(irngs2);
    free(frngs2);
    free(drngs2);
#endif
    
    // Deallocate memory
    free(iseeds);
    free(m);
    free(irngs);
    free(frngs);
    free(drngs);
 
    return 0;
}

