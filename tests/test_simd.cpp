#include "simd.h"
#if defined(SIMD_MODE)


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> // wait
#include <sys/wait.h> // wait
//#include <errno.h>    // errno
#include <unistd.h>  // fork
#include "test_simd.h"


typedef int (*test_handler)();

int test_simd_loadstore();


/*!
 *  Create a child process to run a single test
 */
int test_spawn()
{
  int pfd[2];       // IPC pipes
  int ntests = 0;   // number of tests 
  int *data = NULL; // data buffer
  pid_t mpid, cpid, tpid;  // process IDs 

  // Setup handles to test functions 
  test_handler func_list[] = { test_simd_loadstore }; 

  // Setup process buffer
  ntests = sizeof(func_list) / sizeof(func_list[0]);
  data = (int *)calloc(ntests, sizeof(int));

  // Get master PID
  mpid = getpid();
  printf("Master PID (%d) is running\n", (int)mpid);

  //Create unnamed pipe for IPC
  if (pipe(pfd) == -1) {
    printf("ERROR: failed to create IPC pipes.\n");
    return 0;
  }

  // Fork child process
  cpid = fork();
  if (cpid < 0) {
    printf("ERROR: failed to fork child.\n");
    close(pfd[0]);
    close(pfd[1]);
    return 0;
  }
 
  // Child process
  if (cpid == 0) {
    cpid = getpid();
    printf("Child PID (%d) is running\n", (int)cpid);
   
    // Receive data from master
    read(pfd[0], &ntests, sizeof(int));
    read(pfd[0], data, ntests * sizeof(int));
    for (int i = 0; i < ntests; ++i)
        func_list[data[i]](); 

    // Cleanup
    close(pfd[0]);
    close(pfd[1]);
    return 0;
  
  }  
  // Master process
  else {
    
    // Send data to child 
    for (int i = 0; i < ntests; ++i)
       data[i] = i;
    write(pfd[1], &ntests, sizeof(int));
    write(pfd[1], data, ntests * sizeof(int));

    // Sync child process
    tpid = wait(NULL);
    printf("Master detected child (%d) terminated.\n", (int)tpid);

    // Cleanup
    free(data);
    close(pfd[0]);
    close(pfd[1]);
  }
 
  return 0;
}

// Aligned loads and stores
int test_simd_loadstore()
{
    int npassed = 0;
    int nfailed = 0;
    int *passed = (int *)malloc(SIMD_STREAMS_32 * sizeof(int));
    int *failed = (int *)malloc(SIMD_STREAMS_32 * sizeof(int));

    // Create test arrays
    int *inp_int = NULL;
    int *out_int = NULL;
    posix_memalign((void **)&inp_int, SIMD_WIDTH_BYTES, SIMD_STREAMS_32 * sizeof(int));
    posix_memalign((void **)&out_int, SIMD_WIDTH_BYTES, SIMD_STREAMS_32 * sizeof(int));

    // Initialize test arrays
    for (int i = 0; i < SIMD_STREAMS_32; ++i) {
        inp_int[i] = i * i + 1;
        out_int[i] = 0;
    }

    // Test
    printf("Testing: aligned loads/stores\n");
    SIMD_INT v_int = simd_load(inp_int);
    simd_store(out_int, v_int);

    // Validate test
    for (int i = 0; i < SIMD_STREAMS_32; ++i) {
        if (inp_int[i] == out_int[i])
            passed[npassed++] = i;
        else
            failed[nfailed++] = i;
    }

    // Print results
    printf("passed:");
    for (int i = 0; i < npassed; ++i)
        printf(" %d", passed[i]);
    printf("\n");
    printf("failed:");
    for (int i = 0; i < nfailed; ++i)
        printf(" %d", failed[i]);
    printf("\n");

    // Free test arrays
    free(inp_int);
    free(out_int);

    free(passed);
    free(failed);

    return 0;
}


#endif // SIMD_MODE

