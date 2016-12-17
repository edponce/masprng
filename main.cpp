#include "sse.h"

// Control type of test
#define TEST 0

#if TEST == 0
#define RNG_TYPE_STR "Integer"
#define RNG_TYPE int 
#define get_rn(...) get_rn_int(__VA_ARGS__)
#define VRNG_TYPE SIMD_INT
#define get_vrn(...) get_vrn_int(__VA_ARGS__)
#define RNG_FMT "%d"
#define RNG_ELEMS nstrms32
#define RNG_SHIFT 2
#elif TEST == 1
#define RNG_TYPE_STR "Float"
#define RNG_TYPE float 
#define get_rn(...) get_rn_flt(__VA_ARGS__)
#define VRNG_TYPE SIMD_SP 
#define get_vrn(...) get_vrn_flt(__VA_ARGS__)
#define RNG_FMT "%f"
#define RNG_ELEMS nstrms32
#define RNG_SHIFT 2
#else 
#define RNG_TYPE_STR "Double"
#define RNG_TYPE double
#define get_rn(...) get_rn_dbl(__VA_ARGS__)
#define VRNG_TYPE SIMD_DP 
#define get_vrn(...) get_vrn_dbl(__VA_ARGS__)
#define RNG_FMT "%f"
#define RNG_ELEMS nstrms64
#define RNG_SHIFT 1
#endif

int main()
{
    int i, rval;
    const int nstrms = SIMD_NUM_STREAMS;

    // Initial seeds
    RNG_TYPE *iseeds = NULL;
    rval = posix_memalign((void **)&iseeds, SIMD_ALIGN, nstrms * sizeof(RNG_TYPE));
    for (i = 0; i < nstrms; ++i)
            iseeds[i] = 985456376 - i;

    VRNG_TYPE vdat;
    vdat = vload(iseeds);
    vprint("vdat ", vdat);

    vdat = simd_sll_64(vdat, 1); 
    vprint("vdat ", vdat);

    //vdat = simd_xor(vdat, vdat); 
    //vprint("vdat ", vdat);

    vdat = simd_and(vdat, vdat); 
    vprint("vdat ", vdat);

    return 0;
}

