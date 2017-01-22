#include "simd.h"
#if defined(SIMD_MODE)


#include <stdio.h>
#include <stdlib.h>    // malloc, abs
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
struct TEST_T {
    test_handler test_func;
    const char *test_name;
};

struct TEST_T tests[] =
{
    { test_simd_loadstore, "Aligned loads/stores" },
    { test_simd_loadstore, "Aligned loads/stores" },
    { test_simd_loadstore, "Aligned loads/stores" },
    { test_simd_loadstore, "Aligned loads/stores" },
    { test_simd_loadstore, "Aligned loads/stores" }
}; 


#define MAX_WORKERS 5 


// Pipe management constants
enum PIPE_PORTS { PIPE_READ_PORT = 0, PIPE_WRITE_PORT, NUM_PIPE_PORTS };


/*!
 *  Create pool of worker processes to run test cases.
 *  Tests are accessed via an array of function pointers.
 *
 *  Use pipe for inter-process communication. Pipes provide support
 *  for critical section which is used to control the number of tests
 *  remaining to run.
 *
 *  Test IDs start at 1 to allow +- symmetry for error handling.
 */
int test_spawn()
{
    const int NUM_TESTS = sizeof(tests) / sizeof(struct TEST_T);
    const int NUM_WORKERS = (MAX_WORKERS > NUM_TESTS) ? (NUM_TESTS) : (MAX_WORKERS);
    const int CANCEL_TEST = 0;
    int manager_to_worker[NUM_PIPE_PORTS];
    int worker_to_manager[NUM_PIPE_PORTS];
    pid_t manager_pid;
    pid_t worker_pid = (pid_t)-1; // prevents manager from doing anything if an error occurred
    pid_t wait_pid;
    pid_t *workers = NULL;
    int worker_status;
    int current_test;
    int test_result;
    int *test_results = NULL;

    manager_pid = getpid();

    // Plus 1 since test IDs are offset by 1.
    // Use entry 0 for status of manager process.
    test_results = (int *)malloc((NUM_TESTS + 1) * sizeof(int));
    test_results[0] = 0;

    // Create unnamed pipe for IPC
    if (pipe(manager_to_worker) == -1) {
        printf("(MANAGER %d) ERROR: failed to create manager to worker pipe\n", manager_pid);
        return -1;
    }
    if (pipe(worker_to_manager) == -1) {
        printf("(MANAGER %d) ERROR: failed to create worker to manager pipe\n", manager_pid);
        return -1;
    }

    // Spawn pool of worker processes
    workers = (pid_t *)calloc(NUM_WORKERS, sizeof(pid_t));
    for (int current_worker = 0; current_worker < NUM_WORKERS; ++current_worker) {

        worker_pid = fork();
        if (worker_pid < 0) {
            printf("(MANAGER %d) ERROR: failed to fork worker #%d\n", manager_pid, current_worker + 1);

            // Cancel active workers
            for (int previous_worker = current_worker - 1; previous_worker >= 0; --previous_worker) {
                write(manager_to_worker[PIPE_WRITE_PORT], &CANCEL_TEST, sizeof(int));
            }

            // Wait for active workers to complete
            for (int previous_worker = current_worker - 1; previous_worker >= 0; --previous_worker) {
                wait_pid = waitpid(-1, &worker_status, WUNTRACED | WCONTINUED);
                printf("(MANAGER %d) Worker %d cancelled\n", manager_pid, wait_pid);
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

        // Free/close unused data
        free(workers);
        free(test_results);

        worker_pid = getpid();

        // Receive test ID from manager
        read(manager_to_worker[PIPE_READ_PORT], &current_test, sizeof(int));

        while (current_test > 0) {
            printf("(WORKER  %d) Running test %d ... %s\n", (int)worker_pid, current_test, tests[current_test - 1].test_name);

            // Run test
            test_result = tests[current_test - 1].test_func(); 
            if (test_result != 0)
                current_test = -current_test;

            // Send test result ID to manager
            write(worker_to_manager[PIPE_WRITE_PORT], &current_test, sizeof(int));

            // Receive test ID from manager
            read(manager_to_worker[PIPE_READ_PORT], &current_test, sizeof(int));
        }

        // Close pipes
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
    else if (worker_pid > 0) {
        printf("(MANAGER %d) Total tests = %d\n", manager_pid, NUM_TESTS);
        printf("(MANAGER %d) Total workers = %d\n", manager_pid, NUM_WORKERS);

        // Send test IDs to all workers
        for (current_test = 1; current_test <= NUM_WORKERS; ++current_test) {
            write(manager_to_worker[PIPE_WRITE_PORT], &current_test, sizeof(int));
        }

        // Get test result from worker
        // Send new test ID
        for (; current_test <= NUM_TESTS; ++current_test) {
            read(worker_to_manager[PIPE_READ_PORT], &test_result, sizeof(int));
            test_results[abs(test_result)] = test_result;
            write(manager_to_worker[PIPE_WRITE_PORT], &current_test, sizeof(int));
        }
        
        // Get test result from workers
        // Cancel active workers 
        for (int current_worker = 0; current_worker < NUM_WORKERS; ++current_worker) {
            read(worker_to_manager[PIPE_READ_PORT], &test_result, sizeof(int));
            test_results[abs(test_result)] = test_result;
            write(manager_to_worker[PIPE_WRITE_PORT], &CANCEL_TEST, sizeof(int));
        }

        // Wait for active workers to complete
        for (int current_worker = 0; current_worker < NUM_WORKERS; ++current_worker) {
            wait_pid = waitpid(-1, &worker_status, WUNTRACED | WCONTINUED);
            printf("(MANAGER %d) Worker %d completed\n", manager_pid, wait_pid);
        }

        // Validate test results
        const char *str_result = NULL;
        for (current_test = 1; current_test <= NUM_TESTS; ++current_test) {
            if (test_results[current_test] > 0)
                str_result = "PASSED";
            else
                str_result = "FAILED";
            printf("(MANAGER %d) Test %d %s\n", manager_pid, current_test, str_result);
        }
    }

    // Close pipes
    close(worker_to_manager[PIPE_WRITE_PORT]);
    close(worker_to_manager[PIPE_READ_PORT]);
    close(manager_to_worker[PIPE_WRITE_PORT]);
    close(manager_to_worker[PIPE_READ_PORT]);
 
    free(workers);
    free(test_results);

    return 0;
}


// Aligned loads and stores
int test_simd_loadstore()
{
    int *inp_int = NULL;
    int *out_int = NULL;
    int *failed = NULL;
    int num_failed = 0;
    int test_result = 0;

    // Create test arrays
    posix_memalign((void **)&inp_int, SIMD_WIDTH_BYTES, SIMD_STREAMS_32 * sizeof(int));
    posix_memalign((void **)&out_int, SIMD_WIDTH_BYTES, SIMD_STREAMS_32 * sizeof(int));
    failed = (int *)malloc(SIMD_STREAMS_32 * sizeof(int));

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
    if (num_failed > 0) {
        test_result = -1;
        /*
        printf("FAILED: ");
        printf("failed streams ->");
        for (int i = 0; i < num_failed; ++i)
            printf(" %d", failed[i]);
        printf("\n");
        */
    }

    // Free test arrays
    free(failed);
    free(inp_int);
    free(out_int);

    return test_result;
}


#endif // SIMD_MODE

