#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#include "a2d.h"
#include "eprintf.h"
#include "tspec_diff.h"
#include "mat_mul_pt1_tplt.h"

char *progname = "*** error: 'progname' not set ***";

// forward reference
static double dot(double *a, double *b, int n);

double *da2d_new(int n, int m) {
    double *result = malloc(n * m * sizeof(double));
    assert(result != NULL);
    return result;
}

void da2d_delete(double *d) {
    free(d);
}

void da2d_printf(double *_d, int n, int m, char fmt[]) {
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

void da2d_orthonormalize(double *_a, int n, int m) {
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

void da2d_fillRandom(double *_a, int n, int m) {
#define a(i,j) _a[I2D(i, n, j, m)]
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            a(i,j) = ((double) random()) / RAND_MAX;
        }
    }
#undef a
}

double *da2d_transpose(double *_a, int n, int m) {
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

static double dot(double *a, double *b, int n) {
    double result = 0;
    int i;
    for (i = 0; i < n; i++)
        result += a[i] * b[i];
    return result;
}

static void usage(void) {
    eprintf("usage: %s [{args}]\n", progname);
    eprintf("%s",
            "computes the matrix product c = a x b for two random matrices\n"
            "a[] and b[] of user-specified size\n"
            " {args} are:\n");
    eprintf("%s\n", "  -h      this help message");
    eprintf("%s\n", "  -n {i}  number of rows of c[] and a[] (default: 4)");
    eprintf("%s\n", "  -m {i}  number of columns of c[] and b[] (default: 4)");
    eprintf("%s\n", "  -o      orthonormal a[], b[] is its transpose");
    eprintf("%s\n", "  -p {i}  number of columns of a[] and rows of b[] (default: 4)");
    eprintf("%s\n", "  -s {i}  seed value for srandom");
    eprintf("%s",   "  -v      verbose: print a, b, and a x b\n");
}

int main(int argc, char *argv[]) {
    int n = 4, p = 4, m = 4;
    int verbose = 0, orthonormal = 0;
    int ch, ok = 1;
    double *a, *b, *c;
    int colonIndent = 30;
    struct timespec start_cpu, end_cpu, start_wall, end_wall;

    progname = argv[0];
    while ((ch = getopt(argc, argv, "hm:n:op:qs:v")) != -1) {
        switch (ch) {
        case 'h': usage(); exit(EXIT_SUCCESS);
        case 'n': n = atoi(optarg); break;
        case 'm': m = atoi(optarg); break;
        case 'o': orthonormal = 1; verbose = 1; break;
        case 'p': p = atoi(optarg); break;
        case 's': srandom(atoi(optarg)); break;
        case 'v': verbose = 1; break;
        default: ok = 0; break;
        }
    }
    if (!ok || n <= 0 || p <= 0 || m <= 0) {
        usage(); exit(EXIT_FAILURE);
    }

    if (orthonormal) m = p = n;

    a = da2d_new(n, p);
    da2d_fillRandom(a, n, p);
    if (orthonormal) da2d_orthonormalize(a, n, p);
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

    // Get start time
    clock_gettime(CLOCK_REALTIME, &start_wall);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_cpu);

    // Matrix multiplication
    mat_mul(c, n, m, a, p, b);

    // Get end time
    clock_gettime(CLOCK_REALTIME, &end_wall);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_cpu);

    if (verbose) {
        printf("%*s:\n", colonIndent, "c (= a x b)");
        da2d_printf(c, n, m, "%8.3f ");
        printf("\n");
    }

    // Print timing results
    printf("%*s: %.6f seconds\n", colonIndent, "CPU time", tspecDiff(start_cpu, end_cpu));
    printf("%*s: %.6f seconds\n", colonIndent, "Wall clock time", tspecDiff(start_wall, end_wall));

    da2d_delete(a);
    da2d_delete(b);
    da2d_delete(c);

    return 0;
}
