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

    const int nstrms = SIMD_NUM_STREAMS;

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
    int irngs;

    // Float 
    float frngs;

    // Double 
    double drngs;

    // LCG RNG object
    LCG rng;

    // Initialize RNG params
    rng.init_rng(iseeds[0], m[0]);

#if defined(SIMD_MODE)
    const int nstrms32 = 2 * nstrms;
    const int nstrms64 = nstrms;

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

    // LCG RNG object
    VLCG vrng;

    // Initial RNG params 
    vrng.init_vrng(iseeds, m);
#endif

    // Validate run
    const unsigned int fltmult = 1 << 20, dblmult = 1 << 30; // NOTE: from SPRNG LCG
    int rn = 0;

    // Integer generator
    int valid = 1;
    for (i = 0; i < 200; ++i) {
        rval = scanf("%d\n", &rn);
        irngs = rng.get_rn_int();

        if (rn != irngs) {
            valid = 0;
            printf("File,scalar\t%d\t%d\n", rn, irngs);
        }

#if defined(SIMD_MODE)
        ivrngs = vrng.get_vrn_int();
        simd_store(irngs2, ivrngs);

        if (irngs != irngs2[0]) {
            valid = 0;
            printf("Scalar,vector\t%d\t%d\n", irngs, irngs2[0]);
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
        frngs = rng.get_rn_flt();

        int rn1 = rn >> 11;
        int rn2 = (int)(frngs * fltmult);
        if (abs(rn1 - rn2) > 1) {
            valid = 0;
            printf("File,scalar\t%d\t%d\n", rn1, rn2);
        }

#if defined(SIMD_MODE)
        fvrngs = vrng.get_vrn_flt();
        simd_store(frngs2, fvrngs);

        int rn3 = (int)(frngs2[0] * fltmult);
        if (rn2 != rn3) {
            valid = 0;
            printf("Scalar,vector\t%d\t%d\n", rn2, rn3);
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
        drngs = rng.get_rn_dbl();

        int rn1 = rn >> 1;
        int rn2 = (int)(drngs * dblmult);
        if (abs(rn1 - rn2) > 1) {
            valid = 0;
            printf("File,scalar\t%d\t%d\n", rn1, rn2);
        }

#if defined(SIMD_MODE)
        dvrngs = vrng.get_vrn_dbl();
        simd_store(drngs2, dvrngs);

        int rn3 = (int)(drngs2[0] * dblmult);
        if (rn2 != rn3) {
            valid = 0;
            printf("Scalar,vector\t%d\t%d\n", rn2, rn3);
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
    free(irngs2);
    free(frngs2);
    free(drngs2);
#endif
    
    // Deallocate memory
    free(iseeds);
    free(m);
 
    return 0;
}

