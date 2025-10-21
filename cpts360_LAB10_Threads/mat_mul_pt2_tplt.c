#define _GNU_SOURCE

#include <stdlib.h>     // for malloc()
#include <time.h>       // for clock_gettime
#include <assert.h>
#include <pthread.h>    // for pthreads

#include "a2d.h"
#include "tspec_diff.h"
#include "mat_mul_pt2_tplt.h"

/*
 * Good practice: This structure contains all of the "globals" each
 * thread can access.
 *
 * A pointer to a shared instance of this one struct is shared by all
 * threads used for a single matrix multiply, so any values that
 * might be modified by more than one thread should not be changed by
 * any thread without being protected by some kind of mutex, a la
 * "nextRow".
 */
typedef struct {
    int nextRow;                    // the next row to compute
    pthread_mutex_t nextRowMutex;  // mutex to protect access to nextRow

    int nThreads;                  // number of threads
    int n, m, p;                   // matrix dimensions
    const double *_a, *_b;         // input matrices
    double *_c;                    // output matrix
} ThreadGlobals;


/*
 * multiplyRow -- computes one row of the matrix multiplication result
 */
static void multiplyRow(double *_c, const int i, const int n, const int m,
                        const double *_a, const int p, const double *_b)
{
#define c(i,j) _c[I2D(i, n, j, m)]
#define a(i,j) _a[I2D(i, n, j, p)]
#define b(i,j) _b[I2D(i, p, j, m)]
    int j, k;
    double sum;

    for (j = 0; j < m; j++) {
        sum = 0.0;
        for (k = 0; k < p; k++)
            sum += a(i,k) * b(k,j);
        c(i,j) = sum;
    }
#undef a
#undef b
#undef c
}


/*
 * inThread -- function executed by each pthread
 */
static void *inThread(void *threadGlobals_)
{
    ThreadGlobals *threadGlobals = (ThreadGlobals *) threadGlobals_;
    MatMulThreadStats *matMulThreadStats = malloc(sizeof(MatMulThreadStats));
    matMulThreadStats->nRowsDone = 0;

    // get this thread's CPU clock
    clockid_t cid;
    pthread_getcpuclockid(pthread_self(), &cid);

    struct timespec start_cpu, end_cpu;
    clock_gettime(cid, &start_cpu);  // start CPU time

    while (1) {
        int i;

        // Lock if using multiple threads
        if (threadGlobals->nThreads > 1) {
            pthread_mutex_lock(&threadGlobals->nextRowMutex);
        }

        // Get the next row index
        i = threadGlobals->nextRow;
        threadGlobals->nextRow++;

        // Unlock if using multiple threads
        if (threadGlobals->nThreads > 1) {
            pthread_mutex_unlock(&threadGlobals->nextRowMutex);
        }

        // Check for end of work
        if (i >= threadGlobals->n)
            break;

        // Multiply one row
        multiplyRow(threadGlobals->_c, i, threadGlobals->n, threadGlobals->m,
                    threadGlobals->_a, threadGlobals->p, threadGlobals->_b);
        matMulThreadStats->nRowsDone++;
    }

    // Stop CPU time
    clock_gettime(cid, &end_cpu);
    matMulThreadStats->cpuTime = tspecDiff(start_cpu, end_cpu);

    return matMulThreadStats;
}


/*
 * mat_mul -- launches threads to compute matrix multiplication
 */
void mat_mul(double *_c, const int n, const int m,
             const double *_a, const int p, const double *_b,
             const int nThreads, MatMulThreadStats allMatMulThreadStats[])
{
    // Initialize shared globals
    ThreadGlobals threadGlobals = {
        .nextRow = 0,
        .nThreads = nThreads,
        .n = n,
        .m = m,
        .p = p,
        ._a = _a,
        ._b = _b,
        ._c = _c
    };

    if (nThreads > 0) {
        // Multi-threaded mode
        pthread_t *threads = malloc(sizeof(pthread_t) * nThreads);
        pthread_mutex_init(&threadGlobals.nextRowMutex, NULL);

        // Launch threads
        for (int i = 0; i < nThreads; i++) {
            pthread_create(&threads[i], NULL, inThread, &threadGlobals);
        }

        // Join threads and collect stats
        for (int i = 0; i < nThreads; i++) {
            MatMulThreadStats *result;
            pthread_join(threads[i], (void **) &result);
            allMatMulThreadStats[i] = *result;
            free(result);
        }

        pthread_mutex_destroy(&threadGlobals.nextRowMutex);
        free(threads);
    } else {
        // Single-threaded fallback
        MatMulThreadStats *result = (MatMulThreadStats *) inThread(&threadGlobals);
        allMatMulThreadStats[0] = *result;
        free(result);
    }
}
