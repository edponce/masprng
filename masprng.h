#ifndef __MASPRNG_H
#define __MASPRNG_H


#include "sprng.h"


/*!
 *  Function used to create RNG instances.
 */
SPRNG * selectType(const int);

#if defined(SIMD_MODE)
/*!
 *  Function used to create SIMD RNG instances.
 */
VSPRNG * selectTypeSIMD(const int);
#endif


#endif  // __MASPRNG_H

