#include <stdio.h>
#include <unistd.h> // sysconf
#include "utils.h"


#if defined(_OPENMP)
#include <omp.h> // OpenMP
#endif


/*
 *  Configure OpenMP environment
 */
int setOmpEnv(int *num_threads)
{
    int nt;

#if defined(_OPENMP)
    nt = (num_threads) ? (*num_threads) : OMP_NUM_THREADS;
    omp_set_num_threads(nt);

#if defined(__INTEL_COMPILER)
    setenv("KMP_AFFINITY", "granularity=fine,scatter", 1);
#endif
#else
    nt = (num_threads) ? (*num_threads) : 1;
#endif

    return nt;
}

/*
 *  Detect CPU SIMD features
 */
int printSIMDconf(void)
{
    if (__builtin_cpu_supports("sse"))
        puts("SSE");
    if (__builtin_cpu_supports("sse2"))
        puts("SSE2");
    if (__builtin_cpu_supports("sse3"))
        puts("SSE3");
    if (__builtin_cpu_supports("ssse3"))
        puts("SSSE3");
    if (__builtin_cpu_supports("sse4.1"))
        puts("SSE4.1");
    if (__builtin_cpu_supports("sse4.2"))
        puts("SSE4.2");
    if (__builtin_cpu_supports("avx"))
        puts("AVX");
    if (__builtin_cpu_supports("avx2"))
        puts("AVX2");
    if (__builtin_cpu_supports("avx512f"))
        puts("AVX512F");

    return 0;
}

/*
 *  Print some system configurations
 */
void printSysconf(void)
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

/*
 *  Get the number of processors currently online (available)
 */
long int getNumProcOnline()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
    //return sysconf(_SC_NPROCESSORS_CONF); // number of processors configured
}

/*
 *  Get the size of page in bytes
 */
long int getPageSz()
{
    return sysconf(_SC_PAGESIZE);
    //return sysconf(_SC_PAGE_SIZE);
}

/*
 *  Get the size in bytes of L1 data cache 
 */
long int getL1Sz()
{
    return sysconf(_SC_LEVEL1_DCACHE_SIZE);
}

/*
 *  Get the line size in bytes of L1 data cache
 */
long int getL1LineSz()
{
    return sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
}

/*
 *  Get the associativity of L1 data cache 
 */
long int getL1Assoc()
{
    return sysconf(_SC_LEVEL1_DCACHE_ASSOC);
}

/*
 *  Get the size in bytes of L2 cache 
 */
long int getL2Sz()
{
    return sysconf(_SC_LEVEL2_CACHE_SIZE);
}

/*
 *  Get the line size in bytes of L2 cache
 */
long int getL2LineSz()
{
    return sysconf(_SC_LEVEL2_CACHE_LINESIZE);
}

/*
 *  Get the associativity of L2 cache 
 */
long int getL2Assoc()
{
    return sysconf(_SC_LEVEL2_CACHE_ASSOC);
}

/*
 *  Get the size in bytes of L3 cache 
 */
long int getL3Sz()
{
    return sysconf(_SC_LEVEL3_CACHE_SIZE);
}

/*
 *  Get the line size in bytes of L3 cache
 */
long int getL3LineSz()
{
    return sysconf(_SC_LEVEL3_CACHE_LINESIZE);
}

/*
 *  Get the associativity of L3 cache 
 */
long int getL3Assoc()
{
    return sysconf(_SC_LEVEL3_CACHE_ASSOC);
}

