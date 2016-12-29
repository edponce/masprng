#ifndef __MASPRNG_H
#define __MASPRNG_H


#include "sprng.h"

/*!
 *  Interface used to create RNG instances.
 */
SPRNG * selectType(const int);
#if defined(SIMD_MODE)
VSPRNG * selectTypeSIMD(const int);
#endif


#endif  // __MASPRNG_H

