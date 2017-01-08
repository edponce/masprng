#ifndef __LCG_GLOBALS_H
#define __LCG_GLOBALS_H


// Provides access to alignment attributes required for SIMD mode.
// Misalignment may cause segmentation faults.
#include "simd.h"


// NOTE: requires C++11 for non-static initialization in declaration of struct/class
#if __cplusplus > 199711L


/*!
 *  Global parameters for LCG
 *  All arrays should be aligned for SIMD mode, a macro is provided.
 */
struct LCG_GLOBALS
{
    const char * const GENTYPE = "48 bit Linear Congruential Generator with Prime Addend";
    const int NPARAMS = 7;
#if defined(LONG_SPRNG)
    const long int INIT_SEED = 0x2bc68cfe166dL;
    const long int AN[7] __SIMD_ALIGNED = {0xdadf0ac00001L, 0xfefd7a400001L, 0x6417b5c00001L, 0xcf9f72c00001L, 0xbdf07b400001L, 0xf33747c00001L, 0xcbe632c00001L};
    const long int PMULT[7] __SIMD_ALIGNED = {0xa42c22700000L, 0xfa858cb00000L, 0xd0c4ef00000L, 0xc3cc8e300000L, 0x11bdbe700000L, 0xb0f0e9f00000L, 0x6407de700000L};
    const long int MULT[7] __SIMD_ALIGNED = {0x2875a2e7b175L, 0x5deece66dL, 0x3eac44605265L, 0x275b38eb4bbdL, 0x1ee1429cc9f5L, 0x739a9cb08605L, 0x3228d7cc25f5L};
#else
    const int INIT_SEED[2] __SIMD_ALIGNED = {2868876, 16651885};
    const int MULT[7][4] __SIMD_ALIGNED = {{0x175,0xe7b,0x5a2,0x287}, {0x66d,0xece,0x5de,0x000}, {0x265,0x605,0xc44,0x3ea}, {0x9f5,0x9cc,0x142,0x1ee}, {0xbbd,0xeb4,0xb38,0x275}, {0x605,0xb08,0xa9c,0x739}, {0x5f5,0xcc2,0x8d7,0x322}};
#endif
    const double TWO_M24 = 5.96046447753906234e-8;
    const double TWO_M48 = 3.5527136788005008323e-15;
    const int LCG_RUNUP = 29;
    const int LCG_MAX_STREAMS = 1 << 19;
    const int MSB = 1;
    const int LSB = 0;
};


/*!
 *  Global instance of configuration structure.
 */
const LCG_GLOBALS GLOBALS;


#else // cplusplus


/*!
 *  Global parameters for LCG
 *  All arrays should be aligned for SIMD mode, a macro is provided.
 */
struct LCG_GLOBALS
{
    const char * GENTYPE;
    int NPARAMS;
#if defined(LONG_SPRNG)
    long int INIT_SEED;
    long int AN[7] __SIMD_ALIGNED;
    long int PMULT[7] __SIMD_ALIGNED;
    long int MULT[7] __SIMD_ALIGNED;
#else
    int INIT_SEED[2] __SIMD_ALIGNED;
    int MULT[7][4] __SIMD_ALIGNED;
#endif
    double TWO_M24;
    double TWO_M48;
    int LCG_RUNUP;
    int LCG_MAX_STREAMS;
    int MSB;
    int LSB;
};


/*!
 *  Global instance of configuration structure.
 */
const LCG_GLOBALS GLOBALS = {
    "48 bit Linear Congruential Generator with Prime Addend",
    7,
#if defined(LONG_SPRNG)
    0x2bc68cfe166dL,
    {0xdadf0ac00001L, 0xfefd7a400001L, 0x6417b5c00001L, 0xcf9f72c00001L, 0xbdf07b400001L, 0xf33747c00001L, 0xcbe632c00001L},
    {0xa42c22700000L, 0xfa858cb00000L, 0xd0c4ef00000L, 0xc3cc8e300000L, 0x11bdbe700000L, 0xb0f0e9f00000L, 0x6407de700000L},
    {0x2875a2e7b175L, 0x5deece66dL, 0x3eac44605265L, 0x275b38eb4bbdL, 0x1ee1429cc9f5L, 0x739a9cb08605L, 0x3228d7cc25f5L},
#else
    {2868876, 16651885},
    {{0x175,0xe7b,0x5a2,0x287}, {0x66d,0xece,0x5de,0x000}, {0x265,0x605,0xc44,0x3ea}, {0x9f5,0x9cc,0x142,0x1ee}, {0xbbd,0xeb4,0xb38,0x275}, {0x605,0xb08,0xa9c,0x739}, {0x5f5,0xcc2,0x8d7,0x322}},
#endif
    5.96046447753906234e-8,
    3.5527136788005008323e-15,
    29,
    1 << 19,
    1,
    0
};


#endif // cplusplus


#endif  // __LCG_GLOBALS_H

