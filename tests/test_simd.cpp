#include "simd.h"
#if defined(SIMD_MODE)


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> // wait
#include <sys/wait.h>  // wait
#include <errno.h>     // errno
#include <unistd.h>    // fork
#include <queue>
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


#define NUM_WORKERS 10


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
    const int NUM_TESTS = sizeof(tests) / sizeof(test_handler);
    int manager_to_worker[NUM_PIPE_PORTS];
    int worker_to_manager[NUM_PIPE_PORTS];

    // Create unnamed pipe for IPC
    if (pipe(manager_to_worker) == -1) {
        printf("(MANAGER) ERROR: failed to create manager to worker pipe.\n");
        return -1;
    }
    if (pipe(worker_to_manager) == -1) {
        printf("(MANAGER) ERROR: failed to create worker to manager pipe.\n");
        return -1;
    }

    // Spawn pool of worker processes
    const int CANCEL_TEST = 0;
    pid_t manager_pid, worker_pid, sync_pid;
    pid_t workers[NUM_WORKERS];
    int worker_status;
    for (int current_worker = 0; current_worker < NUM_WORKERS; ++current_worker) {

        worker_pid = fork();
        if (worker_pid < 0) {
            printf("(MANAGER) ERROR: failed to fork worker #%d.\n", current_worker + 1);

            // Cancel all forked worker processes
            printf("(MANAGER) Cancelling forked workers ...\n");
            for (int previous_worker = current_worker - 1; previous_worker >= 0; --previous_worker) {
                write(manager_to_worker[PIPE_WRITE_PORT], &CANCEL_TEST, sizeof(int));
                sync_pid = waitpid(-1, &worker_status, WUNTRACED | WCONTINUED);
                printf("(MANAGER) Worker %d cancelled.\n", sync_pid);
            }

            break; 
        }
        // Worker process should loop out to prevent recursive forks
        else if (worker_pid == 0)
            break;

        workers[current_worker] = worker_pid;
    }

    int current_test;
    
    // Worker process
    if (worker_pid == 0) {

        worker_pid = getpid();
        printf("(WORKER  %d)\n", (int)worker_pid);

        // Receive test ID from manager
        read(manager_to_worker[PIPE_READ_PORT], &current_test, sizeof(int));

        if (current_test == 0) {
            printf("(WORKER  %d) cancelled\n", (int)worker_pid);
            close(worker_to_manager[PIPE_WRITE_PORT]);
            close(worker_to_manager[PIPE_READ_PORT]);
            close(manager_to_worker[PIPE_WRITE_PORT]);
            close(manager_to_worker[PIPE_READ_PORT]);
        }
        else
            printf("(WORKER  %d) still running\n", (int)worker_pid);

        // _exit does not call any functions registered with atexit() or on_exit().
        // Close file descriptors (may flush I/O buffers and remove temporary files)
        _exit(0);
    }
    // Manager process
    else {
        printf("(MANAGER)  Configuration:\n"); 
        printf("           Total tests = %d\n", NUM_TESTS);
        printf("           Total workers = %d\n", NUM_WORKERS);

        manager_pid = getpid();
        printf("(MANAGER %d)\n", (int)manager_pid);

        // Wait for all forked worker processes
        printf("(MANAGER) Waiting for forked workers ...\n");
        for (int current_worker = 0; current_worker < NUM_WORKERS; ++current_worker) {
            write(manager_to_worker[PIPE_WRITE_PORT], &CANCEL_TEST, sizeof(int));
            sync_pid = waitpid(-1, &worker_status, WUNTRACED | WCONTINUED);
            printf("(MANAGER) Worker %d completed.\n", sync_pid);
        }
    }
       
/*    
 
    // Worker  PID = 0
    // Manager PID > 0
    // NOTE: test IDs begin at 1, so +- ID are symmetric
    // If successful, +ID
    // If error, -ID
    int *complete_tests = NULL;
    int result_test = 0;
        // Worker process
        if (current_worker == 0) {
            current_worker = getpid();
            printf("(WORKER %d) ", (int)current_worker);

            // Close pipe ports not used
            close(manager_to_worker[PIPE_WRITE_PORT]);
            close(worker_to_manager[PIPE_READ_PORT]); 
     
            // Receive test ID from manager
            read(manager_to_worker[PIPE_READ_PORT], &current_test, sizeof(int));
            if (current_test != 0) {
                // Run test
                result_test = tests[current_test - 1](); 

                // Send test ID of completed run
                if (result_test == 0)
                    write(worker_to_manager[PIPE_WRITE_PORT], &current_test, sizeof(int));
                else {
                    current_test = -current_test;
                    write(worker_to_manager[PIPE_WRITE_PORT], &current_test, sizeof(int));
                }
            }

            // Close pipe ports not used
            close(manager_to_worker[PIPE_READ_PORT]);
            close(worker_to_manager[PIPE_WRITE_PORT]); 

            return 0;
        }  
    }

        // Manager process
    //    else {
    for (int current_test = 1; current_test <= NUM_TESTS; ++current_test) {
       
            // Track worker's PIDs 
            workers[0] = current_worker; 

            // Close pipe ports not used
            close(manager_to_worker[PIPE_READ_PORT]);
            close(worker_to_manager[PIPE_WRITE_PORT]); 
     
            complete_tests = (int *)calloc(NUM_TESTS, sizeof(int));

            // Send test ID to worker 
            write(manager_to_worker[PIPE_WRITE_PORT], &current_test, sizeof(int));

            // Receive test ID from workers
            read(worker_to_manager[PIPE_READ_PORT], &result_test, sizeof(int));
            if (result_test == 0)
                printf("ERROR: the program should never enter here.\n");
            else if (result_test > 0)
                complete_tests[result_test - 1] = 1;
            else
                complete_tests[-result_test - 1] = 0;

            for (int i = 0; i < NUM_TESTS; ++i) {
                if (complete_tests[i] > 0)
                    printf("test %d successful\n", i+1);
                else
                    printf("test %d unsuccessful\n", i+1);
            }

            // Sync worker processes
            pid_t sync_worker;
            for (int i = 0; i < NUM_WORKERS; ++i) {
                //sync_worker = waitpid(workers[i], &worker_status, WUNTRACED | WCONTINUED);
                sync_worker = waitpid(-1, &worker_status, WUNTRACED | WCONTINUED);
                printf("(MANAGER) Worker %d completed successfully.\n", workers[i]);
            }

            free(complete_tests);
    //    }
   // }
*/

    close(worker_to_manager[PIPE_WRITE_PORT]);
    close(worker_to_manager[PIPE_READ_PORT]);
    close(manager_to_worker[PIPE_WRITE_PORT]);
    close(manager_to_worker[PIPE_READ_PORT]);
 
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

