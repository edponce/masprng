#ifndef __LCG_CONFIG_H
#define __LCG_CONFIG_H


#define GENTYPE "48 bit Linear Congruential Generator with Prime Addend"

#if defined(LONG_SPRNG)
#define INIT_SEED 0x2bc68cfe166dL
#define MSB_SET 0x3ff0000000000000L
#define LSB48 0xffffffffffffL
#define AN1 0xdadf0ac00001L
#define AN2 0xfefd7a400001L
#define AN3 0x6417b5c00001L
#define AN4 0xcf9f72c00001L
#define AN5 0xbdf07b400001L
#define AN6 0xf33747c00001L
#define AN7 0xcbe632c00001L
#define PMULT1 0xa42c22700000L
#define PMULT2 0xfa858cb00000L
#define PMULT3 0xd0c4ef00000L
#define PMULT4 0xc3cc8e300000L
#define PMULT5 0x11bdbe700000L
#define PMULT6 0xb0f0e9f00000L
#define PMULT7 0x6407de700000L
#define NPARAMS 7
#define MULT1 0x2875a2e7b175L
#define MULT2 0x5deece66dL
#define MULT3 0x3eac44605265L
#define MULT4 0x275b38eb4bbdL
#define MULT5 0x1ee1429cc9f5L
#define MULT6 0x739a9cb08605L
#define MULT7 0x3228d7cc25f5L

#else
#define INIT_SEED1 2868876L
#define INIT_SEED2 16651885L
#define NPARAMS 7
// The following is done since the multiplier is stored in pieces of 12 bits each
#define MULT1 {0x175,0xe7b,0x5a2,0x287}
#define MULT2 {0x66d,0xece,0x5de,0x000}
#define MULT3 {0x265,0x605,0xc44,0x3ea}
#define MULT4 {0x9f5,0x9cc,0x142,0x1ee}
#define MULT5 {0xbbd,0xeb4,0xb38,0x275}
#define MULT6 {0x605,0xb08,0xa9c,0x739}
#define MULT7 {0x5f5,0xcc2,0x8d7,0x322}
#endif

#define TWO_M24 5.96046447753906234e-8
#define TWO_M48 3.5527136788005008323e-15

#define LCG_RUNUP 29
#define LCG_MAX_STREAMS (1<<19)

#if defined(LittleEndian)
#define MSB 1
#else
#define MSB 0
#endif
#define LSB (1-MSB)


#define RNG_LONG64_DBL 3.5527136788005008e-15


#endif  // __LCG_CONFIG_H

