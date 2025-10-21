#ifndef _INCLUDED_EPRINTF
#include <stdio.h>  // for fprintf() and stderr
#include <stdlib.h> // for exit()

/*
 * eprintf() is just like printf(), but outputs to stderr instead of
 * stdout.
 *
 * eprintf_fail() is like eprintf() followed by exiting the program as
 * a failure.
 *
 * To emphasize the printf() similarity, we violate the usual practice
 * of putting macros in upper case.
 */

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

#define eprintf_fail(...) \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        exit(EXIT_FAILURE);           \
    } while (0)

#define _INCLUDED_EPRINTF
#endif
