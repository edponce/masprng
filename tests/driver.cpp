#include <stdio.h>
#include "test_simd.h" 


int main()
{
    int tval;

    printf("Testing\n");

    tval = test_simd_loadstore();

    return tval;
}

