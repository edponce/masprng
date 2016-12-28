#ifndef __MASPRNG_H
#define __MASPRNG_H


#include "sprng.h"

/*!
 *  Interface used to create RNG instances.
 */
SPRNG * selectType(int);
#if defined(SIMD_MODE)
VSPRNG * selectTypeSIMD(int);
#endif


#endif  // __MASPRNG_H

