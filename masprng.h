#ifndef __MASPRNG_H
#define __MASPRNG_H


#include "sprng_config.h"


#include "sprng.h"
//#include "lfg.h"
#include "lcg.h"
//#include "lcg64.h"
//#include "cmrg.h"
//#include "mlfg.h"
//#include "pmlcg.h"


/*!
 *  Function used to create RNG instances.
 */
static SPRNG * selectType(const int typenum)
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
    }

    return rng;
}


#include "simd.h"
#if defined(SIMD_MODE)


#include "vsprng.h"
//#include "vlfg.h"
#include "vlcg.h"
//#include "vlcg64.h"
//#include "vcmrg.h"
//#include "vmlfg.h"
//#include "vpmlcg.h"


/*!
 *  Function used to create SIMD RNG instances.
 */
static VSPRNG * selectTypeSIMD(const int typenum)
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
    }

    return rng;
}
#endif


#endif  // __MASPRNG_H

