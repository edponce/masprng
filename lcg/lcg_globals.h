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
    const long int INIT_SEED = 0x2BC68CFE166DL;
    const long int AN[7] = {0xDADF0AC00001L, 0xFEFD7A400001L, 0x6417B5C00001L, 0xCF9F72C00001L, 0xBDF07B400001L, 0xF33747C00001L, 0xCBE632C00001L};
    const long int PMULT[7] = {0xA42C22700000L, 0xFA858CB00000L, 0xD0C4EF00000L, 0xC3CC8E300000L, 0x11BDBE700000L, 0xB0F0E9F00000L, 0x6407DE700000L};
    const long int MULT[7] = {0x2875A2E7B175L, 0x5DEECE66DL, 0x3EAC44605265L, 0x275B38EB4BBDL, 0x1EE1429CC9F5L, 0x739A9CB08605L, 0x3228D7CC25F5L};
#else
    const int INIT_SEED[2] = {2868876, 16651885};
    const int MULT[7][4] = {{0x175,0xE7B,0x5A2,0x287}, {0x66D,0xECE,0x5DE,0x000}, {0x265,0x605,0xC44,0x3EA}, {0x9F5,0x9CC,0x142,0x1EE}, {0xBBD,0xEB4,0xB38,0x275}, {0x605,0xB08,0xA9C,0x739}, {0x5F5,0xCC2,0x8D7,0x322}};
#endif
    const double TWO_M24 = 5.96046447753906234e-8;
    const double TWO_M48 = 3.5527136788005008323e-15;
    const int LCG_RUNUP = 29;
    const int LCG_MAX_STREAMS = 1 << 19;
    const int MSB = 1;
    const int LSB = 0;
} __SIMD_ALIGN__;


/*!
 *  Global instance of configuration structure.
 */
LCG_GLOBALS GLOBALS;


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
    long int AN[7];
    long int PMULT[7];
    long int MULT[7];
#else
    int INIT_SEED[2];
    int MULT[7][4];
#endif
    double TWO_M24;
    double TWO_M48;
    int LCG_RUNUP;
    int LCG_MAX_STREAMS;
    int MSB;
    int LSB;
} __SIMD_ALIGN__;


/*!
 *  Global instance of configuration structure.
 */
const LCG_GLOBALS GLOBALS __SIMD_ALIGN__ = {
    "48 bit Linear Congruential Generator with Prime Addend",
    7,
#if defined(LONG_SPRNG)
    0x2BC68CFE166DL,
    {0xDADF0AC00001L, 0xFEFD7A400001L, 0x6417B5C00001L, 0xCF9F72C00001L, 0xBDF07B400001L, 0xF33747C00001L, 0xCBE632C00001L},
    {0xA42C22700000L, 0xFA858CB00000L, 0xD0C4EF00000L, 0xC3CC8E300000L, 0x11BDBE700000L, 0xB0F0E9F00000L, 0x6407DE700000L},
    {0x2875A2E7B175L, 0x5DEECE66DL, 0x3EAC44605265L, 0x275B38EB4BBDL, 0x1EE1429CC9F5L, 0x739A9CB08605L, 0x3228D7CC25F5L},
#else
    {2868876, 16651885},
    {{0x175,0xE7B,0x5A2,0x287}, {0x66D,0xECE,0x5DE,0x000}, {0x265,0x605,0xC44,0x3EA}, {0x9F5,0x9CC,0x142,0x1EE}, {0xBBD,0xEB4,0xB38,0x275}, {0x605,0xB08,0xA9C,0x739}, {0x5F5,0xCC2,0x8D7,0x322}},
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

