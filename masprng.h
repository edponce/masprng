#ifndef __MASPRNG_H
#define __MASPRNG_H


#include "lcg.h"
#include "lcg_simd.h"


// To select RNG type require SPRNG base and derived classes
#include "sprng.h"
SPRNG * selectType(const int);
#if defined(SIMD_MODE)
VSPRNG * selectVType(const int);
#endif


#endif  // __MASPRNG_H

