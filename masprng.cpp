#include <cstdio>
#include "masprng.h"


//#include "lfg.h"
#include "lcg.h"
//#include "lcg64.h"
//#include "cmrg.h"
//#include "mlfg.h"
//#include "pmlcg.h"


SPRNG * selectType(int typenum)
{
    SPRNG *rng = NULL;

    switch (typenum) {
        //case SPRNG_LFG: rng = new LFG();
        //    break;
        case SPRNG_LCG: rng = new LCG();
            break;
        //case SPRNG_LCG64: rng = new LCG64();
        //    break;
        //case SPRNG_CMRG: rng = new CMRG();
        //    break;
        //case SPRNG_MLFG: rng = new MLFG();
        //    break;
        //case SPRNG_PMLCG: rng = new PMLCG();
        //    break;
        default: printf("ERROR: invalid RNG generator type.\n");
    }

    return rng;
}


#if defined(SIMD_MODE)
//#include "lfg_simd.h"
#include "lcg_simd.h"
//#include "lcg64_simd.h"
//#include "cmrg_simd.h"
//#include "mlfg_simd.h"
//#include "pmlcg_simd.h"



VSPRNG * selectTypeSIMD(int typenum)
{
    VSPRNG *rng = NULL;

    switch (typenum) {
        //case SPRNG_LFG: rng = new VLFG();
        //    break;
        case SPRNG_LCG: rng = new VLCG();
            break;
        //case SPRNG_LCG64: rng = new VLCG64();
        //    break;
        //case SPRNG_CMRG: rng = new VCMRG();
        //    break;
        //case SPRNG_MLFG: rng = new VMLFG();
        //    break;
        //case SPRNG_PMLCG: rng = new VPMLCG();
        //    break;
        default: printf("ERROR: invalid RNG generator type.\n");
    }

    return rng;
}
#endif

