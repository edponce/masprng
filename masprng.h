#ifndef __MASPRNG_H
#define __MASPRNG_H


#include <stdlib.h> // NULL


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


#include "vsprng.h"
//#include "vlfg.h"
#include "vlcg.h"
//#include "vlcg64.h"
//#include "vcmrg.h"
//#include "vmlfg.h"
//#include "vpmlcg.h"


#if defined(SIMD_MODE)


/*!
 *  Function used to create SIMD RNG instances.
 */
static VSPRNG * selectTypeSIMD(const int typenum)
{
    VSPRNG *rng = NULL;

    switch (typenum) {
        //case VSPRNG_LFG: rng = new VLFG();
        //    break;
        case VSPRNG_LCG: rng = new VLCG();
            break;
        //case VSPRNG_LCG64: rng = new VLCG64();
        //    break;
        //case VSPRNG_CMRG: rng = new VCMRG();
        //    break;
        //case VSPRNG_MLFG: rng = new VMLFG();
        //    break;
        //case VSPRNG_PMLCG: rng = new VPMLCG();
        //    break;
    }

    return rng;
}


#endif // SIMD_MODE


#endif  // __MASPRNG_H

