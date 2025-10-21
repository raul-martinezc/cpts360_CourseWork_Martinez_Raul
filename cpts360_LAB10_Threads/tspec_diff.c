#include "tspec_diff.h"

double tspecDiff(struct timespec tspecStart, struct timespec tspecEnd)
/* returns the difference between two timespec structs in seconds */
{
    return (tspecEnd.tv_sec - tspecStart.tv_sec)
        + 1e-9 * (tspecEnd.tv_nsec - tspecStart.tv_nsec);
}
