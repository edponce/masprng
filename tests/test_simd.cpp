#include "simd.h"
#if defined(SIMD_MODE)


#include "test_utils.h"
#include "test_simd.h"


// Aligned loads and stores
int test_simd_loadstore()
{
    int test_result = 0;

    // Integer
    {
        int *arr_A = NULL;
        int *arr_B = NULL;

        create_test_array(TEST_I32, (void **)&arr_A, SIMD_STREAMS_32, SIMD_WIDTH_BYTES);
        create_test_array(TEST_I32, (void **)&arr_B, SIMD_STREAMS_32, SIMD_WIDTH_BYTES);

        SIMD_INT va = simd_load(arr_A);
        simd_store(arr_B, va);

        test_result += validate_test_arrays(TEST_I32, (void *)arr_A, (void *)arr_B, SIMD_STREAMS_32);
        free(arr_A);
        free(arr_B);
    }

    // Float 
    {
        float *arr_A = NULL;
        float *arr_B = NULL;

        create_test_array(TEST_FLT, (void **)&arr_A, SIMD_STREAMS_32, SIMD_WIDTH_BYTES);
        create_test_array(TEST_FLT, (void **)&arr_B, SIMD_STREAMS_32, SIMD_WIDTH_BYTES);

        SIMD_FLT va = simd_load(arr_A);
        simd_store(arr_B, va);

        test_result += validate_test_arrays(TEST_FLT, (void *)arr_A, (void *)arr_B, SIMD_STREAMS_32);
        free(arr_A);
        free(arr_B);
    }

    return test_result;
}


// Floating-point fused multiply-add
int test_simd_fmadd()
{
    int test_result = 0;

    // Float 
    {
        float *arr_A = NULL;
        float *arr_B = NULL;
        float *arr_C1 = NULL;
        float *arr_C2 = NULL;

        create_test_array(TEST_FLT, (void **)&arr_A, SIMD_STREAMS_32, SIMD_WIDTH_BYTES);
        create_test_array(TEST_FLT, (void **)&arr_B, SIMD_STREAMS_32, SIMD_WIDTH_BYTES);
        create_test_array(TEST_FLT, (void **)&arr_C1, SIMD_STREAMS_32, SIMD_WIDTH_BYTES);
        create_test_array(TEST_FLT, (void **)&arr_C2, SIMD_STREAMS_32, SIMD_WIDTH_BYTES);

        SIMD_FLT va = simd_load(arr_A);
        SIMD_FLT vb = simd_load(arr_B);
        SIMD_FLT vc = simd_load(arr_C1);
        vc = simd_fmadd(va, vb, vc);
        simd_store(arr_C1, vc);

        for (int i = 0; i < SIMD_STREAMS_32; ++i)
            arr_C2[i] = arr_A[i] * arr_B[i] + arr_C2[i]; 

        test_result += validate_test_arrays(TEST_FLT, (void *)arr_C1, (void *)arr_C2, SIMD_STREAMS_32);

        free(arr_A);
        free(arr_B);
        free(arr_C1);
        free(arr_C2);
    }

    // Double 
    {
        double *arr_A = NULL;
        double *arr_B = NULL;
        double *arr_C1 = NULL;
        double *arr_C2 = NULL;

        create_test_array(TEST_DBL, (void **)&arr_A, SIMD_STREAMS_64, SIMD_WIDTH_BYTES);
        create_test_array(TEST_DBL, (void **)&arr_B, SIMD_STREAMS_64, SIMD_WIDTH_BYTES);
        create_test_array(TEST_DBL, (void **)&arr_C1, SIMD_STREAMS_64, SIMD_WIDTH_BYTES);
        create_test_array(TEST_DBL, (void **)&arr_C2, SIMD_STREAMS_64, SIMD_WIDTH_BYTES);

        SIMD_DBL va = simd_load(arr_A);
        SIMD_DBL vb = simd_load(arr_B);
        SIMD_DBL vc = simd_load(arr_C1);
        vc = simd_fmadd(va, vb, vc);
        simd_store(arr_C1, vc);

        for (int i = 0; i < SIMD_STREAMS_64; ++i)
            arr_C2[i] = arr_A[i] * arr_B[i] + arr_C2[i]; 

        test_result += validate_test_arrays(TEST_DBL, (void *)arr_C1, (void *)arr_C2, SIMD_STREAMS_32);

        free(arr_A);
        free(arr_B);
        free(arr_C1);
        free(arr_C2);
    }

    return test_result;
}


#endif // SIMD_MODE

