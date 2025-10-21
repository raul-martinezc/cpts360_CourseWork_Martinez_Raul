#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "snapshot.h"

static char *progname = NULL;

static void usage(void);
int snapshot_debug = 0;


int main(int argc, char **argv)
{
    int ch, status;
    extern char *optarg;
    extern int optind;
    int ok = 1;
    char *dnChkpt = "chkpt";

    progname = argv[0];
    while ((ch = getopt(argc, argv, "d?")) != -1) {
        switch (ch) {

        case 'd':
            snapshot_debug = 1;
            break;

        case '?':
            usage();
            exit(EXIT_SUCCESS);
        }
    }
    if (!ok || optind < argc - 1) {
        usage();
        exit(EXIT_FAILURE);
    } else if (optind == argc - 1) {
        dnChkpt = argv[optind];
    }

    if ((status = snapshot(dnChkpt, argv[0],
                        "This is a simple test of snapshot functionality."))
        != 0) {
        fprintf(stderr,
            "Didn't work!  (status = %d)  (is \"ulimit -c\" > 0?)\n", status);
    }

    exit(EXIT_SUCCESS);
}

/* usage: issue a usage error message */
static void usage(void)
{
    (void) fprintf(stderr,
        "usage: %s [{args}] [{snapshot directory name}]\n", progname);
    (void) fprintf(stderr, "%s\n", " {args} are:");
    (void) fprintf(stderr, "%s\n", "  -d  run in debug mode");
    (void) fprintf(stderr, "%s\n", "  -?  this help message");
    return;
}
