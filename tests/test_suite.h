#ifndef __TEST_SUITE_H
#define __TEST_SUITE_H


// Setup handler of tests to run
typedef int (*test_handler)();
struct TEST_T {
    test_handler test_func;
    const char *test_name;
};


// Header files for test cases
#include "test_simd.h"


// Array of test cases
struct TEST_T TESTS[] =
{
    { test_simd_loadstore, "Aligned loads/stores" },
    { test_simd_fmadd, "Fused multiply-add" },
}; 


//////////////////////////////


#define MAX_WORKERS 8 


int test_suite(const struct TEST_T * const, const int, const int);
pid_t create_workers(int * const, const int);
int manager_main(int * const, int * const, const int, const int);
int worker_main(int * const, int * const, const struct TEST_T * const, const int);
int cancel_workers(int * const, const int);


#endif  // __TEST_SUITE_H

