#ifndef INCLUDED_CRITICAL_SECTION

/*
 * Call this function once at the start of the program to initialize
 * (but not enter or leave) the critical section.
 */
extern void criticalSection_init(void);

/*
 * Call this function to enter the critical section, possibly waiting
 * to do so.
 */
extern void criticalSection_enter(void);

/*
 * Call this function to leave the critical section. Be sure you've
 * entered it first!
 */
extern void criticalSection_leave(void);

#define INCLUDED_CRITICAL_SECTION
#endif
