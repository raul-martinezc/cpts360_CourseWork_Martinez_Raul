#include <assert.h>

/*
 *  I2D_C_STD(i, j, m) converts the 2D index (i,j) into a 1D index
 *  into a (1D) C array representing a 2D {anything} x m array using
 *  the normal C multidimensional conventions.  It does not perform
 *  bounds checking.
 */
#define I2D_C_STD(i, j, m) ((m)*(i) + (j))

/*
 *  I2D is our basic 2D indexing macro.  Use it to index all 1D
 *  representations of 2D arrays (usually within other macros).
 *  It includes bounds checking.
 */
#ifdef __GNUC__
/*
 *  If GNU C is available, take advantage of its ability to encode
 *  statements within expressions (note the "({" and "})"), which is
 *  non-standard C, but a very handy idea!
 */
#define I2D(i, n, j, m) ({ \
    /* make temporary copies in case of expressions or side effects */ \
    int _i = (i); \
    int _n = (n); \
    int _j = (j); \
    int _m = (m); \
\
    assert(0 <= _i && _i < _n); \
    assert(0 <= _j && _j < _m); \
    I2D_C_STD(_i, _j, _m); })
#else
/*
 *  Doing bounds checking in inline functions, which are in the C99
 *  standard, is less satisfactory than doing it in macros because
 *  assertion failure yields the location of the assert() in the
 *  inline definition, not the place in the "calling" code where the
 *  bounds check was violated.
 */
static inline int I2D(int i, int n, int j, int m)
{
    assert(0 <= i && i < n);
    assert(0 <= j && j < m);
    return I2D_C_STD(i, j, m);
}
#endif
