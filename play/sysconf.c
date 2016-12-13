#include <unistd.h>


/*
 *  Get the number of processors currently online (available)
 */
long getNumProcOnline()
{
  return sysconf(_SC_NPROCESSORS_ONLN);
  //return sysconf(_SC_NPROCESSORS_CONF); // number of processors configured
}

/*
 *  Get the size of page in bytes
 */
long getPageSz()
{
  return sysconf(_SC_PAGESIZE);
  //return sysconf(_SC_PAGE_SIZE);
}

/*
 *  Get the size in bytes of L1 data cache 
 */
long getL1Sz()
{
  return sysconf(_SC_LEVEL1_DCACHE_SIZE);
}

/*
 *  Get the line size in bytes of L1 data cache
 */
long getL1LineSz()
{
  return sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
}

/*
 *  Get the associativity of L1 data cache 
 */
long getL1Assoc()
{
  return sysconf(_SC_LEVEL1_DCACHE_ASSOC);
}

/*
 *  Get the size in bytes of L2 cache 
 */
long getL2Sz()
{
  return sysconf(_SC_LEVEL2_CACHE_SIZE);
}

/*
 *  Get the line size in bytes of L2 cache
 */
long getL2LineSz()
{
  return sysconf(_SC_LEVEL2_CACHE_LINESIZE);
}

/*
 *  Get the associativity of L2 cache 
 */
long getL2Assoc()
{
  return sysconf(_SC_LEVEL2_CACHE_ASSOC);
}

/*
 *  Get the size in bytes of L3 cache 
 */
long getL3Sz()
{
  return sysconf(_SC_LEVEL3_CACHE_SIZE);
}

/*
 *  Get the line size in bytes of L3 cache
 */
long getL3LineSz()
{
  return sysconf(_SC_LEVEL3_CACHE_LINESIZE);
}

/*
 *  Get the associativity of L3 cache 
 */
long getL3Assoc()
{
  return sysconf(_SC_LEVEL3_CACHE_ASSOC);
}


#include <stdio.h>

int main()
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

  return 0;
}

