#include "a2d.h"
#include "tspec_diff.h"
#include "mat_mul_pt1_tplt.h"


static void multiplyRow(double *_c, const int i, const int n, const int m,
                        const double *_a, const int p, const double *_b)
{
#define c(i,j) _c[I2D(i, n, j, m)]
#define a(i,j) _a[I2D(i, n, j, p)]
#define b(i,j) _b[I2D(i, p, j, m)]
    int j, k;
    double sum;

    sum = 0.0;
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


void mat_mul(double *_c, const int n, const int m,
             const double *_a, const int p, const double *_b)
{
    int i;

    for (i = 0; i < n; i++)
        multiplyRow(_c, i, n, m, _a, p, _b);
}

