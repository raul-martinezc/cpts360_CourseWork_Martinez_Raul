#ifndef _A2D_H_INCLUDED
#define _A2D_H_INCLUDED

#include <assert.h>
/*
 *  At some future point, we might have an A2D_BOUNDS_CHECK() macro
 *  that works differently from assert(), raising an IndexError
 *  exception, for instance.  For now, note that we can distinguish
 *  indices that are too small from those that are too large.
 *
 *  It would be good practice to create temporary int variables inside
 *  these macros to avoid side effects and run a little faster, but
 *  that would prevent assert from using the actual names of the
 *  indices in its error message.
 */
#define A2D_BOUNDS_CHECK(i, n) do { \
    assert(0 <= i); \
    assert(i < n); \
} while (0)

/*
 *  I2D_C_STD(i, j, m) converts the 2D index (i,j) to a 1D index into
 *  a (1D) C array representing a 2D {anything} x m array using the
 *  normal C multidimensional conventions.  It does not perform bounds
 *  checking.
 */
#define I2D_C_STD(i, j, m) ((m)*(i) + (j))

/*
 *  I2D is our basic 2D indexing macro.  Use it to index all 1D
 *  representations of 2D arrays (usually within other macros).
 *  It includes bounds checking.
 */
# ifdef __GNUC__
/*
 *  If GNU C is available, take advantage of its ability to encode
 *  statements within expressions (note the "({" and "})"), which is
 *  non-standard C, but a very handy idea!
 *
 *  (See above comments on not using temporary variables inside these
 *  macros.)
 */
# define I2D(i, n, j, m) ({ \
\
    A2D_BOUNDS_CHECK(i, n); \
    A2D_BOUNDS_CHECK(j, m); \
    I2D_C_STD(i, j, m); })
# else
/*
 *  Doing bounds checking in inline functions, which are in the C99
 *  standard, is less satisfactory than doing it in macros because
 *  assertion failure, if enabled, prints on stderr the location of
 *  the assert() in the inline definition, not the place in the
 *  "calling" code where the bounds check was violated.
 */
static inline int I2D(int i, int n, int j, int m)
{
    A2D_BOUNDS_CHECK(i, n); \
    A2D_BOUNDS_CHECK(i, m); \
    return I2D_C_STD(i, j, m);
}
# endif

#endif // _A2D_H_INCLUDED
