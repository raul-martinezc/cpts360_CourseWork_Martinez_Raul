/*
 * plotz2_pt2 -- Dynamically allocated 2D z-value image generator
 */

 #include <stdio.h>
 #include <math.h>
 #include <assert.h>
 #include "allocarray.h"
 
 int nPixel1D = 512; // global image size (default)
 
 void getExtrema(double *_z, double *zMin_p, double *zMax_p)
 /* return the minimum and maximum values in a 2D double array */
 {
 #define z(i,j) _z[nPixel1D*(i)+(j)]
     int i, j;
 
     (*zMin_p) = (*zMax_p) = z(0, 0); // initialize
     for (i = 0; i < nPixel1D; i++) {
         for (j = 0; j < nPixel1D; j++) {
             if (z(i,j) < (*zMin_p)) {
                 (*zMin_p) = z(i,j); // update minimum
             } else if ((*zMax_p) < z(i,j)) {
                 (*zMax_p) = z(i,j); // update maximum
             }
         }
     }
 #undef z
 }
 
 void printSquarePgm(double *_z)
 /* print a double array on stdout as a PGM file with automatic scaling */
 {
 #define z(i,j) _z[nPixel1D*(i)+(j)]
     double zMin, zMax;
     int i, j;
     int pixelValue;
     int maxVal = 255;
 
     getExtrema(_z, &zMin, &zMax);
 
     printf("P2\n");
     printf("%d %d\n", nPixel1D, nPixel1D);
     printf("%d\n", maxVal);
     for (i = 0; i < nPixel1D; i++) {
         for (j = 0; j < nPixel1D; j++) {
             if (zMin == zMax) {
                 pixelValue = 128;
             } else {
                 pixelValue = maxVal * (z(i,j) - zMin) / (zMax - zMin);
             }
             printf("%3d ", pixelValue);
         }
         printf("\n");
     }
 #undef z
 }
 
 void sampleFunction(double (*f_p)(double x, double y), double *_z)
 // sample an nPixel1D x nPixel1D grid over the square [-1,1] x [-1,1]
 {
 #define z(i,j) _z[nPixel1D*(i)+(j)]
     double x, dx, y, dy;
     int i, j;
 
     dx = 2.0 / (nPixel1D - 1);
     dy = 2.0 / (nPixel1D - 1);
     for (i = 0; i < nPixel1D; i++) {
         y = 1.0 - dy * i;
         for (j = 0; j < nPixel1D; j++) {
             x = dx * j - 1.0;
             z(i,j) = (*f_p)(x, y);
         }
     }
 #undef z
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
 
 /* Add your own function here, if you wish. */
 
 int main(int argc, char *argv[])
 {
     double *z;
     ALLOC_ARRAY(z, double, nPixel1D * nPixel1D);
 
     sampleFunction(hemisphere, z);
 
     printSquarePgm(z);
 
     FREE_ARRAY(z);
     return 0;
 }
 