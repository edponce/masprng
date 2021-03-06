#include <stdio.h>
#include <stdlib.h>
#include "masprng.h"
#include "check.h"


/*!
 *  Check errors with SPRNG data output found in file
 */
int check_gen(const int rng_type)
{
    int i;
    int iread;
    const int nstrms = SIMD_STREAMS_32;

    // Initial seeds
    int iseeds[nstrms];
    for (i = 0; i < nstrms; ++i)
        iseeds[i] = 985456376 - i;

    // Initial multiplier indices
    int m[nstrms];
    for (i = 0; i < nstrms; ++i)
        m[i] = 0;

    // Scalar
    int irngs;
    float frngs;
    double drngs;

    // RNG object
    SPRNG *rng = selectType(rng_type);
	if (!rng)
		return -1;
    rng->init_rng(0, 1, iseeds[0], m[0]);

#if defined(SIMD_MODE)
    // SIMD
    int irngs2[nstrms];
    float frngs2[nstrms];
    double drngs2[nstrms];

    SIMD_INT ivrngs;
    SIMD_FLT fvrngs;
    SIMD_DBL dvrngs;

    // RNG object
    VSPRNG *vrng = selectTypeSIMD(rng_type);
	if (!vrng)
		return -1;
    vrng->init_rng(0, 1, iseeds, m, nstrms);
#endif


    const unsigned int fltmult = 1 << 20, dblmult = 1 << 30;
    int rn = 0;

    // Integer generator
    int valid = 1;
    for (i = 0; i < 200; ++i) {
        iread = scanf("%d\n", &rn);
        if (iread == EOF) {
            valid = 0;
            perror("Failed to scan integer generator");
            break;
        }
        irngs = rng->get_rn_int();

        if (rn != irngs) {
            valid = 0;
            printf("File,scalar\t%d\t%d\n", rn, irngs);
        }

#if defined(SIMD_MODE)
        ivrngs = vrng->get_rn_int();
        simd_storeu(irngs2, ivrngs);

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
        iread = scanf("%d\n", &rn);
        if (iread == EOF) {
            valid = 0;
            perror("Failed to scan float generator");
            break;
        }
        frngs = rng->get_rn_flt();

        int rn1 = rn >> 11;
        int rn2 = (int)(frngs * fltmult);
        if (abs(rn1 - rn2) > 1) {
            valid = 0;
            printf("File,scalar\t%d\t%d\n", rn1, rn2);
        }

#if defined(SIMD_MODE)
        fvrngs = vrng->get_rn_flt();
        simd_storeu(frngs2, fvrngs);

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
        iread = scanf("%d\n", &rn);
        if (iread == EOF) {
            valid = 0;
            perror("Failed to scan double generator");
            break;
        }
        drngs = rng->get_rn_dbl();

        int rn1 = rn >> 1;
        int rn2 = (int)(drngs * dblmult);
        if (abs(rn1 - rn2) > 1) {
            valid = 0;
            printf("File,scalar\t%d\t%d\n", rn1, rn2);
        }

#if defined(SIMD_MODE)
        dvrngs = vrng->get_rn_dbl();
        simd_storeu(drngs2, dvrngs);

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
    // Clean SPRNG objects
    delete vrng;
#endif
    delete rng;

    return 0;
}

