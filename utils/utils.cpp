#include <stdio.h>
#include <stdlib.h> // getenv
#include <unistd.h> // sysconf
#include "utils.h"


#define GNUC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)


#if defined(_OPENMP)
    #include <omp.h>
#endif


/*
 *  If parameter is less than 1, the environment variable OMP_NUM_THREADS is considered.
 */
int setOmpEnv(const int num_threads)
{
    int nt;

#if defined(_OPENMP)
    if (getenv("OMP_NUM_THREADS"))
        nt = (num_threads > 0) ? (num_threads) : OMP_NUM_THREADS;
    else
        nt = (num_threads > 0) ? (num_threads) : 1;
    omp_set_num_threads(nt);

# if defined(__INTEL_COMPILER)
    setenv("KMP_AFFINITY", "granularity=fine,scatter", 1);
# endif
#else
    nt = (num_threads > 0) ? (num_threads) : 1;
#endif

    return nt;
}


#if GNUC_VERSION > 40800
#define CPUsupports(a) (__builtin_cpu_supports(a)) ? (1) : (0)
#else
#define CPUsupports(a) 0 // NOTE: assume SIMD support is not available if no way of checking
#endif
int detectProcSIMD()
{
    int support = 1;

#if defined(__FMA__) // NOTE: not sure of this macro
    if (!CPUsupports("fma"))
        support = 0;
#endif
#if defined(__SSE__)
    if (!CPUsupports("sse"))
        support = 0;
#endif
#if defined(__SSE2__)
    if (!CPUsupports("sse2"))
        support = 0;
#endif
#if defined(__SSE3__)
    if (!CPUsupports("sse3"))
        support = 0;
#endif
#if defined(__SSSE3__)
    if (!CPUsupports("ssse3"))
        support = 0;
#endif
#if defined(__SSE4_1__)
    if (!CPUsupports("sse4.1"))
        support = 0;
#endif
#if defined(__SSE4_2__)
    if (!CPUsupports("sse4.2"))
        support = 0;
#endif
#if defined(__AVX__)
    if (!CPUsupports("avx"))
        support = 0;
#endif
#if defined(__AVX2__)
    if (!CPUsupports("avx2"))
        support = 0;
#endif
#if defined(__AVX512BW__)
    if (!CPUsupports("avx512BW"))
        support = 0;
#endif

    return support;
}


void printSysconf()
{
    printf("Number of processors online = %ld\n", getNumProcOnline());
    printf("Page size = %ld\n", getPageSz());

    printf("L1 data cache size = %ld\n", getL1Sz());
    printf("L1 data cache associativity = %ld\n", getL1LineSz());
    printf("L1 data cache line size = %ld\n", getL1Assoc());

    printf("L2 cache size = %ld\n", getL2Sz());
    printf("L2 cache associativity = %ld\n", getL2LineSz());
    printf("L2 cache line size = %ld\n", getL2Assoc());

    printf("L3 cache size = %ld\n", getL3Sz());
    printf("L3 cache associativity = %ld\n", getL3LineSz());
    printf("L3 cache line size = %ld\n", getL3Assoc());

#if defined(_OPENMP)
    printf("OMP threads = %d of %d\n", omp_get_num_threads(), omp_get_max_threads());
#endif
}


long int getNumProcOnline()
{ return sysconf(_SC_NPROCESSORS_ONLN); }
//{ return sysconf(_SC_NPROCESSORS_CONF); }

long int getPageSz()
{ return sysconf(_SC_PAGESIZE); }
//{ return sysconf(_SC_PAGE_SIZE); }

long int getL1Sz()
{ return sysconf(_SC_LEVEL1_DCACHE_SIZE); }

long int getL1LineSz()
{ return sysconf(_SC_LEVEL1_DCACHE_LINESIZE); }

long int getL1Assoc()
{ return sysconf(_SC_LEVEL1_DCACHE_ASSOC); }

long int getL2Sz()
{ return sysconf(_SC_LEVEL2_CACHE_SIZE); }

long int getL2LineSz()
{ return sysconf(_SC_LEVEL2_CACHE_LINESIZE); }

long int getL2Assoc()
{ return sysconf(_SC_LEVEL2_CACHE_ASSOC); }

long int getL3Sz()
{ return sysconf(_SC_LEVEL3_CACHE_SIZE); }

long int getL3LineSz()
{ return sysconf(_SC_LEVEL3_CACHE_LINESIZE); }

long int getL3Assoc()
{ return sysconf(_SC_LEVEL3_CACHE_ASSOC); }

