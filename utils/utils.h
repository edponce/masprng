#ifndef __UTILS_H
#define __UTILS_H


/*!
 *  Configure OpenMP environment
 */
int setOmpEnv(const int);

/*!
 *  Detect CPU SIMD features
 *  Define order has __GNUC__ last because other compilers may define it.
 */
#if defined(__INTEL_COMPILER)
int detectIntelProcSIMD();
#define detectProcSIMD() detectIntelProcSIMD()
#elif defined(__PGI)
int detectPGIProcSIMD();
#define detectProcSIMD() detectPGIProcSIMD()
#elif defined(__GNUC__)
int detectGCCProcSIMD();
#define detectProcSIMD() detectGCCProcSIMD()
#endif

/*!
 *  Print some system configurations
 */
void printSysconf();

/*!
 *  Get the number of processors currently online (available)
 */
long int getNumProcOnline();

/*!
 *  Get the size of page in bytes
 */
long int getPageSz();

/*!
 *  Get the size in bytes of L1 data cache
 */
long int getL1Sz();

/*!
 *  Get the line size in bytes of L1 data cache
 */
long int getL1LineSz();

/*!
 *  Get the associativity of L1 data cache
 */
long int getL1Assoc();

/*!
 *  Get the size in bytes of L2 cache
 */
long int getL2Sz();

/*!
 *  Get the line size in bytes of L2 cache
 */
long int getL2LineSz();

/*!
 *  Get the associativity of L2 cache
 */
long int getL2Assoc();

/*!
 *  Get the size in bytes of L3 cache
 */
long int getL3Sz();

/*!
 *  Get the line size in bytes of L3 cache
 */
long int getL3LineSz();

/*!
 *  Get the associativity of L3 cache
 */
long int getL3Assoc();


#endif  // __UTILS_H

