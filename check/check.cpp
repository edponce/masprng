#include <cstdio>
#include <cstdlib>
#include "masprng.h"
#include "utils.h"
#include "check.h"


#if !defined(SIMD_MODE)
#define SIMD_WIDTH_BYTES 8
#define SIMD_STREAMS_32 1
#define SIMD_STREAMS_64 1
#endif


/*!
 *  Check errors with SPRNG data output found in file
 */
int check_gen(const int rng_type)
{
    int i;
    int rval;

    const int nstrms = SIMD_STREAMS_64;

    // Initial seeds
    int *iseeds = NULL;
    rval = posix_memalign((void **)&iseeds, SIMD_WIDTH_BYTES, nstrms * sizeof(int));
    for (i = 0; i < nstrms; ++i)
        iseeds[i] = 985456376 - i;

    // Initial multiplier indices 
    int *m = NULL;
    rval = posix_memalign((void **)&m, SIMD_WIDTH_BYTES, nstrms * sizeof(int));
    for (i = 0; i < nstrms; ++i)
        m[i] = 0;

    // Scalar
    int irngs;
    float frngs;
    double drngs;

    // RNG object
    SPRNG *rng = selectType(rng_type);
    rng->init_rng(0, 1, iseeds[0], m[0]);

#if defined(SIMD_MODE)
    // SIMD
    int *irngs2 = NULL;
    rval = posix_memalign((void **)&irngs2, SIMD_WIDTH_BYTES, SIMD_STREAMS_32 * sizeof(int));
    float *frngs2 = NULL;
    rval = posix_memalign((void **)&frngs2, SIMD_WIDTH_BYTES, SIMD_STREAMS_32 * sizeof(float));
    double *drngs2 = NULL;
    rval = posix_memalign((void **)&drngs2, SIMD_WIDTH_BYTES, SIMD_STREAMS_64 * sizeof(double));

    SIMD_INT ivrngs;
    SIMD_FLT fvrngs;
    SIMD_DBL dvrngs;

    // RNG object
    VSPRNG *vrng = selectTypeSIMD(rng_type);
    vrng->init_rng(0, 1, iseeds, m);
#endif


    const unsigned int fltmult = 1 << 20, dblmult = 1 << 30;
    int rn = 0;

    // Integer generator
    int valid = 1;
    for (i = 0; i < 200; ++i) {
        rval = scanf("%d\n", &rn);
        irngs = rng->get_rn_int();

        if (rn != irngs) {
            valid = 0;
            printf("File,scalar\t%d\t%d\n", rn, irngs);
        }

#if defined(SIMD_MODE)
        ivrngs = vrng->get_rn_int();
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
        frngs = rng->get_rn_flt();

        int rn1 = rn >> 11;
        int rn2 = (int)(frngs * fltmult);
        if (abs(rn1 - rn2) > 1) {
            valid = 0;
            printf("File,scalar\t%d\t%d\n", rn1, rn2);
        }

#if defined(SIMD_MODE)
        fvrngs = vrng->get_rn_flt();
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
        drngs = rng->get_rn_dbl();

        int rn1 = rn >> 1;
        int rn2 = (int)(drngs * dblmult);
        if (abs(rn1 - rn2) > 1) {
            valid = 0;
            printf("File,scalar\t%d\t%d\n", rn1, rn2);
        }

#if defined(SIMD_MODE)
        dvrngs = vrng->get_rn_dbl();
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
    free(irngs2);
    free(frngs2);
    free(drngs2);
#endif

    free(iseeds);
    free(m);
 
    return rval;
}

