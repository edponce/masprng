#include <stdio.h>
#include "masprng.h"


SPRNG * selectType(const int typenum)
{
    SPRNG * rng = NULL;

    switch (typenum) {
        //case SPRNG_LFG: rng = new LFG();
        case SPRNG_LCG: rng = new LCG();
        //case SPRNG_LCG64: rng = new LCG64();
        //case SPRNG_CMRG: rng = new CMRG();
        //case SPRNG_MLFG: rng = new MLFG();
        //case SPRNG_PMLCG: rng = new PMLCG();
        default: printf("Invalid generator type selected.\n");
    }

    return rng;
}


#if defined(SIMD_MODE)
VSPRNG * selectVType(const int typenum)
{
    VSPRNG * rng = NULL;

    switch (typenum) {
        //case SPRNG_LFG: rng = new VLFG();
        case SPRNG_LCG: rng = new VLCG();
        //case SPRNG_LCG64: rng = new VLCG64();
        //case SPRNG_CMRG: rng = new VCMRG();
        //case SPRNG_MLFG: rng = new VMLFG();
        //case SPRNG_PMLCG: rng = new VPMLCG();
        default: printf("Invalid generator type selected.\n");
    }

    return rng;
}
#endif

