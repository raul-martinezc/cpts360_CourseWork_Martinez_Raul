#ifndef _INCLUDED_ALLOCARRAY
#define _INCLUDED_ALLOCARRAY
#include <stdlib.h> /* imports *alloc() and free() declarations */
#include <assert.h> /* imports assert() macro */

/* allocate an array with n members of type typ */
#define ALLOC_ARRAY(ptr, typ, n) \
    { ptr = (typ *) malloc((n) * sizeof(typ)); assert(ptr); }

#define ALLOC_STRUCT(ptr, typ)            ALLOC_ARRAY(ptr, typ, 1)

/* allocate an array with n members of type typ, initialized to zero */
#define CALLOC_ARRAY(ptr, typ, n) \
    { ptr = (typ *) calloc((n), sizeof(typ)); assert(ptr); }

/* free malloc'd memory w/pointer safeguard */
#define FREE_ARRAY(ptr) \
    { assert(ptr); free((void *) (ptr)); (ptr) = NULL; }

// synonym for consistency
#define FREE_STRUCT(ptr) FREE_ARRAY(ptr)

/* resize a previously-allocated array to have n members of type typ,
 * preserving contents */
#define REALLOC_ARRAY(ptr, typ, n) \
    { ptr = (typ *) realloc((void *) ptr, (n) * sizeof(typ)); assert(ptr); }

/*
 *   Assume that a null pointer (presumed to indicate an unallocated
 *   block of memory) requires malloc() and a non-null one requires
 *   realloc().
 */
#define RESIZE_ARRAY(ptr, typ, n) \
    { if (ptr) { REALLOC_ARRAY(ptr, typ, n) } else { ALLOC_ARRAY(ptr, typ, n) } }

#endif /* _INCLUDED_ALLOCARRAY */
