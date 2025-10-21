#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <sys/time.h>
#include <time.h> // for `clock_gettime()`

#include "a2d.h"
#include "eprintf.h"
#include "tspec_diff.h"
#include "mat_mul_pt2_tplt.h"

char *progname = "*** error: 'progname' not set ***";


// forward reference
static double dot(double *a, double *b, int n);


double *da2d_new(int n, int m)
{
    double *result = malloc(n * m * sizeof(double));
    assert(result != NULL);
    return result;
}


void da2d_delete(double *d)
{
    free(d);
}


void da2d_printf(double *_d, int n, int m, char fmt[])
{
#define d(i,j) _d[I2D(i, n, j, m)]
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            printf(fmt, d(i,j));
        }
        printf("\n");
    }
#undef d
}


void da2d_orthonormalize(double *_a, int n, int m)
{
#define a(i,j) _a[I2D(i, n, j, m)]
    int i, iPrev, j;
    double dotProduct, mag;

    for (i = 0; i < n; i++) {
        for (iPrev = 0; iPrev < i; iPrev++) {
            dotProduct = dot(&a(iPrev,0), &a(i,0), m);
            for (j = 0; j < m; j++) {
                a(i,j) -= dotProduct * a(iPrev,j);
            }
        }
        mag = sqrt(dot(&a(i,0), &a(i,0), m));
        for (j = 0; j < m; j++) {
            a(i,j) /= mag;
        }
    }
#undef a
}


void da2d_fillRandom(double *_a, int n, int m)
{
#define a(i,j) _a[I2D(i, n, j, m)]
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            a(i,j) = ((double) random()) / RAND_MAX;
        }
    }
#undef a
}


double *da2d_transpose(double *_a, int n, int m)
{
#define a(i,j) _a[I2D(i, n, j, m)]
#define b(i,j) _b[I2D(i, m, j, n)]
    double *_b = da2d_new(m, n);
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            b(j,i) = a(i,j);
        }
    }
    return _b;
#undef b
#undef a
}


static double dot(double *a, double *b, int n)
{
    double result = 0;
    int i;
    for (i = 0; i < n; i++)
        result += a[i] * b[i];
    return result;
}


static void usage(void)
{
    eprintf("usage: %s [{args}]\n", progname);
    eprintf("%s",
            "computes the matrix product c = a x b for two random matrices\n"
            "a[] and b[] of user-specified size, optionally with threads,\n"
            "and prints the times involved\n"
            " {args} are:\n");
    eprintf("%s\n",
            "  -h      this help message");
    eprintf("%s\n",
            "  -n {i}  number of rows of c[] and a[] (default: 4)");
    eprintf("%s\n",
            "  -m {i}  number of columns of c[] and b[] (default: 4)");
    eprintf("%s\n",
            "  -o      test algorithm by forcing a[] to be orthonormal and b[]\n"
            "          to be its transpose (implies '-v' and n == m == p; default: don't)");
    eprintf("%s\n",
            "  -p {i}  number of columns of a[] and rows of b[] (default: 4)");
    eprintf("%s\n",
            "  -s {i}  seed value for srandom (default: no seeding)");
    eprintf("%s\n",
            "  -t {i}  number of threads (0 means unthreaded) (default: 4)");
    eprintf("%s",
            "  -v      verbose: print out a and b as well as their product\n");
    return;
}


int main(int argc, char *argv[])
{
    int n = 4;
    int p = 4;
    int m = 4;
    int verbose = 0;
    int orthonormal = 0;
    int ch, ok;
    double *a, *b, *c;
    int colonIndent;
    int nThreads = 4;
    int i;
    MatMulThreadStats *matMulThreadStats;
    double cpuTimeTotal;

    ok = 1;
    progname = argv[0];
    while ((ch = getopt(argc, argv, "hm:n:op:qs:t:v")) != -1) {
        switch (ch) {
        case 'h': usage(); exit(EXIT_SUCCESS);
        case 'n': n = atoi(optarg); break;
        case 'o': verbose = 1; orthonormal = 1; break;
        case 'm': m = atoi(optarg); break;
        case 'p': p = atoi(optarg); break;
        case 's': srandom(atoi(optarg)); break;
        case 't': nThreads = atoi(optarg); break;
        case 'v': verbose = 1; break;
        default: ok = 0; break;
        }
    }
    if (!ok || n <= 0 || p <= 0  || m <= 0 || nThreads < 0) {
        usage();
        exit(EXIT_FAILURE);
    }

    colonIndent = 30;

    if (orthonormal) {
        m = p = n;
    }

    a = da2d_new(n, p);
    da2d_fillRandom(a, n, p);
    if (orthonormal) {
        da2d_orthonormalize(a, n, p);
    }
    if (verbose) {
        printf("%*s:\n", colonIndent, "a");
        da2d_printf(a, n, p, "%8.3f ");
        printf("\n");
    }

    if (orthonormal) {
        b = da2d_transpose(a, n, p);
    } else {
        b = da2d_new(p, m);
        da2d_fillRandom(b, p, m);
    }
    if (verbose) {
        printf("%*s:\n", colonIndent, "b");
        da2d_printf(b, p, m, "%8.3f ");
        printf("\n");
    }

    c = da2d_new(n, m);

    // === Start timing ===
    struct timespec start_wall, end_wall, start_cpu, end_cpu;
    clock_gettime(CLOCK_REALTIME, &start_wall);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_cpu);

    int actualThreads = (nThreads == 0) ? 1 : nThreads;
    matMulThreadStats = malloc(sizeof(MatMulThreadStats) * actualThreads);

    mat_mul(c, n, m, a, p, b, nThreads, matMulThreadStats);

    // === End timing ===
    clock_gettime(CLOCK_REALTIME, &end_wall);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_cpu);

    if (verbose) {
        printf("%*s:\n", colonIndent, "c (= a x b)");
        da2d_printf(c, n, m, "%8.3f ");
        printf("\n");
    }

    if (nThreads == 0) {
        printf("%*s: %.6f seconds\n", colonIndent, "Thread[0] CPU time",
               matMulThreadStats[0].cpuTime);
    } else {
        cpuTimeTotal = 0.0;
        for (i = 0; i < nThreads; i++) {
            printf("%*s: %.6f seconds, rows done = %d\n",
                   colonIndent, "Thread CPU time", matMulThreadStats[i].cpuTime,
                   matMulThreadStats[i].nRowsDone);
            cpuTimeTotal += matMulThreadStats[i].cpuTime;
        }
        printf("%*s: %.6f seconds\n", colonIndent, "Sum of thread CPU times", cpuTimeTotal);
    }

    printf("%*s: %.6f seconds\n", colonIndent, "CPU time",
           tspecDiff(start_cpu, end_cpu));
    printf("%*s: %.6f seconds\n", colonIndent, "Wall clock time",
           tspecDiff(start_wall, end_wall));

    free(matMulThreadStats);
    da2d_delete(a);
    da2d_delete(b);
    da2d_delete(c);

    return 0;
}
