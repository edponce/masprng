#include <stdio.h>
#include "masprng.h"


SPRNG * MASPRNG::selectType(int typenum)
{
    SPRNG * rng = NULL;

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
        default: printf("Invalid generator type selected.\n");
    }

    return rng;
}


#if defined(SIMD_MODE)
VSPRNG * MASPRNG::selectTypeSIMD(int typenum)
{
    VSPRNG * rng = NULL;

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
        default: printf("Invalid generator type selected.\n");
    }

    return rng;
}
#endif

