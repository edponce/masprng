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
    test_simd_loadstore,
    test_simd_loadstore,
    test_simd_loadstore,
    test_simd_loadstore,
    test_simd_loadstore
}; 


#define MAX_WORKERS 1 


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
    const int NUM_WORKERS = (MAX_WORKERS > NUM_TESTS) ? (NUM_TESTS) : (MAX_WORKERS);
    const int CANCEL_TEST = 0;
    int manager_to_worker[NUM_PIPE_PORTS];
    int worker_to_manager[NUM_PIPE_PORTS];
    pid_t manager_pid, worker_pid, wait_pid;
    pid_t *workers = NULL;
    int worker_status;

    // Create unnamed pipe for IPC
    if (pipe(manager_to_worker) == -1) {
        printf("(MANAGER %d) ERROR: failed to create manager to worker pipe.\n", manager_pid);
        return -1;
    }
    if (pipe(worker_to_manager) == -1) {
        printf("(MANAGER %d) ERROR: failed to create worker to manager pipe.\n", manager_pid);
        return -1;
    }

    // Spawn pool of worker processes
    workers = (pid_t *)calloc(NUM_WORKERS, sizeof(pid_t));
    for (int current_worker = 0; current_worker < NUM_WORKERS; ++current_worker) {

        worker_pid = fork();
        if (worker_pid < 0) {
            printf("(MANAGER %d) ERROR: failed to fork worker #%d.\n", manager_pid, current_worker + 1);

            // Cancel all forked worker processes
            for (int previous_worker = current_worker - 1; previous_worker >= 0; --previous_worker) {
                write(manager_to_worker[PIPE_WRITE_PORT], &CANCEL_TEST, sizeof(int));
                wait_pid = waitpid(-1, &worker_status, WUNTRACED | WCONTINUED);
                printf("(MANAGER %d) Worker %d cancelled.\n", manager_pid, wait_pid);
            }

            break; 
        }
        // Worker process should loop out to prevent recursive forks
        else if (worker_pid == 0)
            break;

        workers[current_worker] = worker_pid;
    }

    // Worker process
    if (worker_pid == 0) {
        
        int current_test;
        int result_test;

        worker_pid = getpid();
        printf("(WORKER  %d) Active\n", (int)worker_pid);

        // Receive test ID from manager
        read(manager_to_worker[PIPE_READ_PORT], &current_test, sizeof(int));

        while (current_test > 0) {
            printf("(WORKER  %d) Running test %d ...\n", (int)worker_pid, current_test);

            // Run test
            result_test = tests[current_test - 1](); 
            if (result_test != 0)
                current_test = -current_test;

            // Send test result ID to manager
            write(worker_to_manager[PIPE_WRITE_PORT], &current_test, sizeof(int));

            // Receive test ID from manager
            read(manager_to_worker[PIPE_READ_PORT], &current_test, sizeof(int));
        }

        printf("(WORKER  %d) Cancelled by manager ...\n", (int)worker_pid);
        close(worker_to_manager[PIPE_WRITE_PORT]);
        close(worker_to_manager[PIPE_READ_PORT]);
        close(manager_to_worker[PIPE_WRITE_PORT]);
        close(manager_to_worker[PIPE_READ_PORT]);

        // Flush C user-space buffers to disk
        fflush(stdout);

        // Flush kernel buffers to disk
        fsync(fileno(stdout));

        // _exit does not call any functions registered with atexit() or on_exit().
        // Close file descriptors (may flush I/O buffers and remove temporary files)
        _exit(0);
    }
    // Manager process
    // Distributes tests among workers using a fair approach, FCFS (first-come, first-served)
    else {
        int current_test;
        int result_test;

        manager_pid = getpid();
        printf("(MANAGER %d) Total tests = %d\n", manager_pid, NUM_TESTS);
        printf("(MANAGER %d) Total workers = %d\n", manager_pid, NUM_WORKERS);

        // Send test IDs to all workers
        for (current_test = 1; current_test <= NUM_WORKERS; ++current_test) {
            write(manager_to_worker[PIPE_WRITE_PORT], &current_test, sizeof(int));
        }

        // Get test result from a worker
        // Send a remaining test ID
        for (; current_test <= NUM_TESTS; ++current_test) {
            read(worker_to_manager[PIPE_READ_PORT], &result_test, sizeof(int));
            write(manager_to_worker[PIPE_WRITE_PORT], &current_test, sizeof(int));
        }
        
        // Get test result from all workers
        // Cancel all workers 
        for (int current_worker = 0; current_worker < NUM_WORKERS; ++current_worker) {
            read(worker_to_manager[PIPE_READ_PORT], &result_test, sizeof(int));
            write(manager_to_worker[PIPE_WRITE_PORT], &CANCEL_TEST, sizeof(int));
        }

        for (int current_worker = 0; current_worker < NUM_WORKERS; ++current_worker) {
            wait_pid = waitpid(-1, &worker_status, WUNTRACED | WCONTINUED);
            printf("(MANAGER %d) Worker %d completed.\n", manager_pid, wait_pid);
        }
    }

    close(worker_to_manager[PIPE_WRITE_PORT]);
    close(worker_to_manager[PIPE_READ_PORT]);
    close(manager_to_worker[PIPE_WRITE_PORT]);
    close(manager_to_worker[PIPE_READ_PORT]);
 
    free(workers);

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
    int result_test = 0;

    pid_t worker_pid = getpid();
    printf("(WORKER  %d)  %s\n", worker_pid, test_name);

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
    for (int i = 0; i < SIMD_STREAMS_32; ++i) {
        if (inp_int[i] != out_int[i])
            failed[num_failed++] = i;
    }

    // Print results
    if (num_failed == 0) {
        result_test = 0;
        //printf("PASSED\n");
    }
    else {
        result_test = -1;
        /*
        printf("FAILED\n");
        printf("\tfailed streams:");
        for (int i = 0; i < num_failed; ++i)
            printf(" %d", failed[i]);
        printf("\n");
        */
    }

    // Free test arrays
    free(failed);
    free(inp_int);
    free(out_int);

    return result_test;
}


#endif // SIMD_MODE

