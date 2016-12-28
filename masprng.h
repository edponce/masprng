#ifndef __MASPRNG_H
#define __MASPRNG_H


#include "lcg.h"

#if defined(SIMD_MODE)
#include "lcg_simd.h"
#endif


#include "sprng.h"
class MASPRNG
{
  public:
    static SPRNG * selectType(int); 
#if defined(SIMD_MODE)
    static VSPRNG * selectTypeSIMD(int); 
#endif
};


#endif  // __MASPRNG_H

