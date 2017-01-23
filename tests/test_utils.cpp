#include <stdlib.h>    // posix_memalign
#include <float.h>     // floating-point epsilons
#include "test_utils.h"


void create_test_array(const int arr_type, void ** const arr, const int num_elems, const int alignment)
{
    // Array allocation
    switch (arr_type) {
        case TEST_U32:
            posix_memalign((void **)arr, alignment, num_elems * sizeof(unsigned int));
            break;
        case TEST_U64:
            posix_memalign((void **)arr, alignment, num_elems * sizeof(long unsigned int));
            break;
        case TEST_I32:
            posix_memalign((void **)arr, alignment, num_elems * sizeof(int));
            break;
        case TEST_I64:
            posix_memalign((void **)arr, alignment, num_elems * sizeof(long int));
            break;
        case TEST_FLT:
            posix_memalign((void **)arr, alignment, num_elems * sizeof(float));
            break;
        case TEST_DBL:
            posix_memalign((void **)arr, alignment, num_elems * sizeof(double));
    }

    // Array initialization
    switch (arr_type) {
        case TEST_U32:
            for (int i = 0; i < num_elems; ++i)
                ((unsigned int *)*arr)[i] = i + 1;
            break;
        case TEST_U64:
            for (int i = 0; i < num_elems; ++i)
                ((long unsigned int *)*arr)[i] = i + 1;
            break;
        case TEST_I32:
            for (int i = 0; i < num_elems; ++i)
                ((int *)*arr)[i] = i - num_elems/2;
            break;
        case TEST_I64:
            for (int i = 0; i < num_elems; ++i)
                ((long int *)*arr)[i] = i - num_elems/2;
            break;
        case TEST_FLT:
            for (int i = 0; i < num_elems; ++i)
                ((float *)*arr)[i] = i * 3.14;
            break;
        case TEST_DBL:
            for (int i = 0; i < num_elems; ++i)
                ((double *)*arr)[i] = i * 3.14;
    }
}


// Validate test
int validate_test_arrays(const int arr_type, const void * const arr1, const void * const arr2, const int num_elems)
{
    int num_failed = 0;

    switch (arr_type) {
        case TEST_U32:
            for (int i = 0; i < num_elems; ++i)
                if (((unsigned int *)arr1)[i] != ((unsigned int *)arr2)[i])
                    num_failed++;
            break;
        case TEST_U64:
            for (int i = 0; i < num_elems; ++i)
                if (((long unsigned int *)arr1)[i] != ((long unsigned int *)arr2)[i])
                    num_failed++;
            break;
        case TEST_I32:
            for (int i = 0; i < num_elems; ++i)
                if (((int *)arr1)[i] != ((int *)arr2)[i])
                    num_failed++;
            break;
        case TEST_I64:
            for (int i = 0; i < num_elems; ++i)
                if (((long int *)arr1)[i] != ((long int *)arr2)[i])
                    num_failed++;
            break;
        case TEST_FLT:
            for (int i = 0; i < num_elems; ++i)
                if ((((float *)arr1)[i] - ((float *)arr2)[i]) > FLT_EPSILON)
                    num_failed++;
            break;
        case TEST_DBL:
            for (int i = 0; i < num_elems; ++i)
                if ((((double *)arr1)[i] - ((double *)arr2)[i]) > DBL_EPSILON)
                    num_failed++;
    }

    return num_failed;
}

