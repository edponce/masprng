#include "simd.h"
#if defined(SIMD_MODE)


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> // wait
#include <sys/wait.h>  // wait
#include <errno.h>     // errno
#include <unistd.h>    // fork
#include "test_simd.h"


// Test prototype
int test_simd_loadstore();


// Setup handler of tests to run
typedef int (*test_handler)();
test_handler tests[] =
{
    test_simd_loadstore,
    test_simd_loadstore
}; 


#define NUM_WORKERS 4


// Pipe management constants
enum PIPE_PORTS { PIPE_READ_PORT = 0, PIPE_WRITE_PORT, NUM_PIPE_PORTS };


/*!
 *  Create pool of worker processes to run test cases requested.
 *  Tests are available via an array of function pointers.
 *
 *  Use pipe for inter-process communication. Pipes provide support
 *  for critical section which is used to control the number of tests
 *  remaining to run.
 */
int test_spawn()
{
    pid_t master, current_worker;
    pid_t workers[NUM_WORKERS];
    int master_to_worker[NUM_PIPE_PORTS];
    int worker_to_master[NUM_PIPE_PORTS];
    int current_test = 0;
    int num_tests = sizeof(tests) / sizeof(test_handler);

    // Get master PID
    master = getpid();
    printf("(MASTER)  Configuration:\n"); 
    printf("           Total tests = %d\n", num_tests);
    printf("           Total workers = %d\n", NUM_WORKERS);

    //Create unnamed pipe for IPC
    if (pipe(master_to_worker) == -1) {
        printf("(MASTER) ERROR: failed to create master to worker pipe.\n");
        return 0;
    }
    if (pipe(worker_to_master) == -1) {
        printf("(MASTER) ERROR: failed to create worker to master pipe.\n");
        return 0;
    }

    // Close pipe ports not used
    close(master_to_worker[PIPE_READ_PORT]);
    close(worker_to_master[PIPE_WRITE_PORT]);

    current_test = 0;

    // Spawn worker process
    workers[0] = fork();
    current_worker = workers[0]; 
    if (current_worker < 0) {
        printf("(MASTER) ERROR: failed to fork worker.\n");
        close(master_to_worker[PIPE_WRITE_PORT]);
        close(worker_to_master[PIPE_READ_PORT]);

        return 0;
    }
 
    // Worker process
    if (current_worker == 0) {
        current_worker = getpid();
        printf("(WORKER %d) ", 0);

        // Close pipe ports not used
        close(master_to_worker[PIPE_WRITE_PORT]);
        close(worker_to_master[PIPE_READ_PORT]); 
     
        // Receive num_tests from master
        read(master_to_worker[PIPE_READ_PORT], &current_test, sizeof(int));
        tests[current_test](); 

        return 0;
    }  

    // Master process
    else {
    
        // Send num_tests to worker 
        write(master_to_worker[PIPE_WRITE_PORT], &current_test, sizeof(int));

        // Sync worker processes
        pid_t sync_worker;
        int worker_status;
        for (int i = 0; i < NUM_WORKERS; ++i) {
            sync_worker = waitpid(current_worker, &worker_status, WUNTRACED | WCONTINUED);
            printf("(MASTER) Worker %d completed successfully.\n", 0);
        }

        // Close IPC pipes
        close(master_to_worker[PIPE_WRITE_PORT]);
        close(worker_to_master[PIPE_READ_PORT]);
    }
 
    return 0;
}


// Aligned loads and stores
int test_simd_loadstore()
{
    const char *test_name = "Aligned loads/stores";
    int num_failed = 0;
    int *failed = NULL;
    int *inp_int = NULL;
    int *out_int = NULL;

    printf("%s ... ", test_name);

    // Create test arrays
    failed = (int *)malloc(SIMD_STREAMS_32 * sizeof(int));
    posix_memalign((void **)&inp_int, SIMD_WIDTH_BYTES, SIMD_STREAMS_32 * sizeof(int));
    posix_memalign((void **)&out_int, SIMD_WIDTH_BYTES, SIMD_STREAMS_32 * sizeof(int));

    // Initialize test arrays
    for (int i = 0; i < SIMD_STREAMS_32; ++i) {
        failed[i] = -1;
        inp_int[i] = i * i + 1;
        out_int[i] = 0;
    }

    // Test
    SIMD_INT v_int = simd_load(inp_int);
    simd_store(out_int, v_int);

    // Validate test
    for (int i = 0; i < SIMD_STREAMS_32; ++i)
        if (inp_int[i] != out_int[i])
            failed[num_failed++] = i;

    // Print results
    if (num_failed == 0)
        printf("PASSED\n");
    else {
        printf("FAILED\n");
        printf("\tfailed streams:");
        for (int i = 0; i < num_failed; ++i)
            printf(" %d", failed[i]);
        printf("\n");
    }

    // Free test arrays
    free(failed);
    free(inp_int);
    free(out_int);

    return 0;
}


#endif // SIMD_MODE

