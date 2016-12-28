#ifndef __LCG_CONFIG_H
#define __LCG_CONFIG_H


struct LCG_CONFIG {
    const char * const GENTYPE = "48 bit Linear Congruential Generator with Prime Addend";
    int const NPARAMS = 7;

#if defined(LONG_SPRNG)
    long int const INIT_SEED = 0x2bc68cfe166dL;
    long int const MSB_SET = 0x3ff0000000000000L;
    long int const LSB48 = 0xffffffffffffL;
    long int const AN1 = 0xdadf0ac00001L;
    long int const AN2 = 0xfefd7a400001L;
    long int const AN3 = 0x6417b5c00001L;
    long int const AN4 = 0xcf9f72c00001L;
    long int const AN5 = 0xbdf07b400001L;
    long int const AN6 = 0xf33747c00001L;
    long int const AN7 = 0xcbe632c00001L;
    long int const PMULT1 = 0xa42c22700000L;
    long int const PMULT2 = 0xfa858cb00000L;
    long int const PMULT3 = 0xd0c4ef00000L;
    long int const PMULT4 = 0xc3cc8e300000L;
    long int const PMULT5 = 0x11bdbe700000L;
    long int const PMULT6 = 0xb0f0e9f00000L;
    long int const PMULT7 = 0x6407de700000L;
    long int const MULT[7] = {0x2875a2e7b175L,
                              0x5deece66dL,
                              0x3eac44605265L,
                              0x275b38eb4bbdL,
                              0x1ee1429cc9f5L,
                              0x739a9cb08605L,
                              0x3228d7cc25f5L};
#else
    const int INIT_SEED[2] = {2868876, 16651885};
    // The following is done since the multiplier is stored in pieces of 12 bits each
    const int MULT[7][4] = {{0x175,0xe7b,0x5a2,0x287},
                            {0x66d,0xece,0x5de,0x000},
                            {0x265,0x605,0xc44,0x3ea},
                            {0x9f5,0x9cc,0x142,0x1ee},
                            {0xbbd,0xeb4,0xb38,0x275},
                            {0x605,0xb08,0xa9c,0x739},
                            {0x5f5,0xcc2,0x8d7,0x322}};
#endif
    
    const double TWO_M24 = 5.96046447753906234e-8;
    const double TWO_M48 = 3.5527136788005008323e-15;

    const int LCG_RUNUP = 29;
    const int LCG_MAX_STREAMS = 1<<19;

#if defined(LittleEndian)
    const int MSB = 1;
    const int LSB = 0;
#else
    const int MSB = 0;
    const int LSB = 1;
#endif
    const double RNG_LONG64_DBL = 3.5527136788005008e-15;
};


LCG_CONFIG const CONFIG;


#endif  // __LCG_CONFIG_H

