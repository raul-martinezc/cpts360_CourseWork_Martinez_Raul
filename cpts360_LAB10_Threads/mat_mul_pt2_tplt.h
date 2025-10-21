/*
 * Statistics for thread computation are kept in an array of these,
 * one per thread.
 */
typedef struct {
    double cpuTime;
    int nRowsDone;
} MatMulThreadStats;

void mat_mul(double *_c, const int n, const int m,
             const double *_a, const int p, const double *_b,
             const int nThreads, MatMulThreadStats *matMulThreadStats);
