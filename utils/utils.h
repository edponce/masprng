#ifndef __UTILS_H
#define __UTILS_H


/*
 *  Configure OpenMP environment
 */
int setOmpEnv(int *);

/*
 *  Detect CPU SIMD features
 */
int printSIMDconf(void);

/*
 *  Print some system configurations
 */
void printSysconf(void);

/*
 *  Get the number of processors currently online (available)
 */
long int getNumProcOnline(void);

/*
 *  Get the size of page in bytes
 */
long int getPageSz(void);

/*
 *  Get the size in bytes of L1 data cache 
 */
long int getL1Sz(void);

/*
 *  Get the line size in bytes of L1 data cache
 */
long int getL1LineSz(void);

/*
 *  Get the associativity of L1 data cache 
 */
long int getL1Assoc(void);

/*
 *  Get the size in bytes of L2 cache 
 */
long int getL2Sz(void);

/*
 *  Get the line size in bytes of L2 cache
 */
long int getL2LineSz(void);

/*
 *  Get the associativity of L2 cache 
 */
long int getL2Assoc(void);

/*
 *  Get the size in bytes of L3 cache 
 */
long int getL3Sz(void);

/*
 *  Get the line size in bytes of L3 cache
 */
long int getL3LineSz(void);

/*
 *  Get the associativity of L3 cache 
 */
long int getL3Assoc(void);


#endif // __UTILS_H

