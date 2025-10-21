/*
 * plotz2 -- 2D z-value image generator
 */

#include <stdio.h>
#include <math.h>

#define N_PIXEL_1D 7

void getExtrema(double z[N_PIXEL_1D][N_PIXEL_1D], double *zMin_p,
                double *zMax_p)
/* return the minimum and maximum values in a 2D double array */
{
    /*
     * input
     *    z[][] -- an N_PIXEL_1D x N_PIXEL_1D array of double values
     *
     * output
     *    *zMin_p - the minimum value in z[][]
     *    *zMax_p - the maximum value in z[][]
     */
    int i, j;

    (*zMin_p) = (*zMax_p) = z[0][0]; // initialize
    for (i = 0; i < N_PIXEL_1D; i++) { // for all rows i
        for (j = 0; j < N_PIXEL_1D; j++) { // for all columns j
            if (z[i][j] < (*zMin_p)) {
                (*zMin_p) = z[i][j]; // update minimum
            } else if ((*zMax_p) < z[i][j]) {
                (*zMax_p) = z[i][j]; // update maximum
            }
        }
    }
}

void printSquarePgm(double z[N_PIXEL_1D][N_PIXEL_1D])
/* print a double array on stdout as a PGM file with automatic scaling */
{
    /*
     * input
     *    z[][] -- an N_PIXEL_1D x N_PIXEL_1D array of double values
     *
     * This function first finds the maximum and minimum values in
     * z[][], zMin and zMax, with a call to getExtrema(). It then
     * prints out the values of the array as a PGM file. The PGM file
     * format is as follows:
     *
     * line #  contains
     * ------  --------
     *   1      "P2"
     *   2      "w h" where "w" is the width of the image and "h" is the
     *           height. In this case, both values are N_PIXEL_1D.
     *   3      "255"
     *   4      scaled (see below) values of z[0][...]
     *   5      scaled values of z[1][...]
     *  ...     ...
     *  N_PIXEL_1D+3     scaled values of z[N_PIXEL_1D-1][...]
     *
     * Before printing, each value of z[][] is scaled and converted to
     * an int "pixelValue" between 0 and 255 (inclusively).
     */
    double zMin, zMax;
    int i, j;
    int pixelValue;
    int maxVal = 255; /* should be at least this, greater is okay */

    getExtrema(z, &zMin, &zMax);

    printf("P2\n");
    printf("%d %d\n", N_PIXEL_1D, N_PIXEL_1D);
    printf("%d\n", maxVal);
    for (i = 0; i < N_PIXEL_1D; i++) {
        for (j = 0; j < N_PIXEL_1D; j++) {
            if (zMin == zMax) {
                pixelValue = 128;
            } else {
                pixelValue = maxVal * (z[i][j] - zMin) / (zMax - zMin);
            }
            printf("%3d ", pixelValue);
        }
        printf("\n");
    }
}

void sampleFunction(double (*f_p)(double x, double y),
                    double z[N_PIXEL_1D][N_PIXEL_1D])
// sample an N_PIXEL_1D x N_PIXEL_1D grid over the square
// [-1,1] x [-1,1] */
{
    /*
     * input:
     *    f -- pointer to a function that will be called over the grid
     *
     * output:
     *    z -- N_PIXEL_1D x N_PIXEL_1D array of values of f
     *         evaluated over [ -1, 1 ] x [ -1, 1 ]. z[0][0]
     *         corresponds to the upper left corner (x, y) = (-1, 1).
     *
     * This function evaluates an N_PIXEL_1D x N_PIXEL_1D grid that
     * fits into a 2 x 2 square centered on the origin.
     */
    double x, dx, y, dy;
    int i, j;

    dx = 2.0 / (N_PIXEL_1D - 1);
    dy = 2.0 / (N_PIXEL_1D - 1);
    for (i = 0; i < N_PIXEL_1D; i++) {
        y = 1.0 - dy * i;
        for (j = 0; j < N_PIXEL_1D; j++) {
            x = dx * j - 1.0;
            z[i][j] = (*f_p)(x, y);
        }
    }
}

double hemisphere(double x, double y)
/* return the height of a unit hemisphere or 0 if (x,y) lies outside */
{
    double rSqrd = 1.0 - x*x - y*y;
    if (rSqrd > 0)
        return sqrt(rSqrd);
    return 0.0;
}

double ripple(double x, double y)
/* return a radial, exponentially-damped cosine, or "ripple" */
{
    double r = sqrt(x*x + y*y);
    return exp(-2*r) * cos(20*r);
}

/*
 * Add your own function here, if you wish. Be creative!
 */

int main(int argc, char *argv[])
{
    /*
     * Windows won't let us declare local (i.e. stack) variables this
     * large (Linux will -- so there!), so we need to declare them
     * static (i.e. global) for portability.
     */
    static double z[N_PIXEL_1D][N_PIXEL_1D];

    // Instead of "hemisphere", try "ripple" or your own function's
    // name.
    sampleFunction(hemisphere, z);

    printSquarePgm(z);

    return 0;
}
