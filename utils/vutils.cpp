#include "simd.h"
#if defined(SIMD_MODE)


#include <stdio.h>
#include <stdlib.h> // posix_memalign
#include <errno.h> // errno
#include "vutils.h" 


int scalar_malloc(int ** const sa, const size_t align, const size_t nelems)
{
	int ierr = posix_memalign((void **)sa, align, nelems * sizeof(int));
	if (ierr)
		printf("ERROR: failed to allocate aligned memory, %d\n", errno);
	return ierr;
}


int scalar_malloc(unsigned int ** const sa, const size_t align, const size_t nelems)
{
	int ierr = posix_memalign((void **)sa, align, nelems * sizeof(unsigned int));
	if (ierr)
		printf("ERROR: failed to allocate aligned memory, %d\n", errno);
	return ierr;
}


int scalar_malloc(long int ** const sa, const size_t align, const size_t nelems)
{
	int ierr = posix_memalign((void **)sa, align, nelems * sizeof(long int));
	if (ierr)
		printf("ERROR: failed to allocate aligned memory, %d\n", errno);
	return ierr;
}


int scalar_malloc(unsigned long int ** const sa, const size_t align, const size_t nelems)
{
	int ierr = posix_memalign((void **)sa, align, nelems * sizeof(unsigned long int));
	if (ierr)
		printf("ERROR: failed to allocate aligned memory, %d\n", errno);
	return ierr;
}


int scalar_malloc(float ** const sa, const size_t align, const size_t nelems)
{
	int ierr = posix_memalign((void **)sa, align, nelems * sizeof(float));
	if (ierr)
		printf("ERROR: failed to allocate aligned memory, %d\n", errno);
	return ierr;
}


int scalar_malloc(double ** const sa, const size_t align, const size_t nelems)
{
	int ierr = posix_memalign((void **)sa, align, nelems * sizeof(double));
	if (ierr)
		printf("ERROR: failed to allocate aligned memory, %d\n", errno);
	return ierr;
}


int simd_malloc(SIMD_INT ** const va, const size_t align, const size_t nelems)
{
	int ierr = posix_memalign((void **)va, align, nelems * sizeof(SIMD_INT));
	if (ierr)
		printf("ERROR: failed to allocate aligned memory, %d\n", errno);
	return ierr;
}


int simd_malloc(SIMD_FLT ** const va, const size_t align, const size_t nelems)
{
	int ierr = posix_memalign((void **)va, align, nelems * sizeof(SIMD_FLT));
	if (ierr)
		printf("ERROR: failed to allocate aligned memory, %d\n", errno);
	return ierr;
}


int simd_malloc(SIMD_DBL ** const va, const size_t align, const size_t nelems)
{
	int ierr = posix_memalign((void **)va, align, nelems * sizeof(SIMD_DBL));
	if (ierr)
		printf("ERROR: failed to allocate aligned memory, %d\n", errno);
	return ierr;
}


void simd_print(const char * const str, const SIMD_INT va)
{
    int32_t itmp[SIMD_STREAMS_32] __SIMD_ALIGN__;
    simd_store(itmp, va);
    printf("%s", str);
    for (int32_t i = 0; i < SIMD_STREAMS_32; ++i)
      printf("%d\t", itmp[i]);
    printf("\n");

    uint32_t utmp[SIMD_STREAMS_32] __SIMD_ALIGN__; 
    simd_store(utmp, va);
    printf("%s", str);
    for (int32_t i = 0; i < SIMD_STREAMS_32; ++i)
      printf("%u\t", utmp[i]);
    printf("\n");

    uint64_t lutmp[SIMD_STREAMS_64] __SIMD_ALIGN__;
    simd_store(lutmp, va);
    printf("%s", str);
    for (int32_t i = 0; i < SIMD_STREAMS_64; ++i)
      printf("%lu\t", lutmp[i]);
    printf("\n");
}


void simd_print(const char * const str, const SIMD_FLT va)
{
    float tmp[SIMD_STREAMS_32] __SIMD_ALIGN__;
    simd_store(tmp, va);
    printf("%s", str);
    for (int32_t i = 0; i < SIMD_STREAMS_32; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}


void simd_print(const char * const str, const SIMD_DBL va)
{
    double tmp[SIMD_STREAMS_64] __SIMD_ALIGN__;
    simd_store(tmp, va);
    printf("%s", str);
    for (int32_t i = 0; i < SIMD_STREAMS_64; ++i)
      printf("%f\t", tmp[i]);
    printf("\n");
}


#endif // SIMD_MODE

