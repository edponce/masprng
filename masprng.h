#ifndef __MASPRNG_H
#define __MASPRNG_H


// OpenMP
#if defined(_OPENMP)
#define SCHED_OMP static // static, dynamic, guided, runtime
#define CHUNK_OMP 8         // chunk size of scheduling option
#ifndef OMP_NUM_THREADS
#define OMP_NUM_THREADS 8   // number of threads for OpenMP
#endif
#endif


// Scalar mode
#define NSTRMS 1
#define VECTOR_ALIGN 8

#include "lcg.h"


#endif  // __MASPRNG_H

