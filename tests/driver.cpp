#include <stdio.h>
#include "test_simd.h" 


int main()
{
    int tval;

    printf("SIMD TEST SUITE\n");
    tval = test_simd_loadstore();

    tval = test_spawn();

    return tval;
}

