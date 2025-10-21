#include <stdlib.h>   // for exit()
#include <stdio.h>    // for printf(), perror(), etc.
#include <getopt.h>   // for getopt()
#include <string.h>   // for strcpy(), snprintf(), strsignal()
#include <unistd.h>   // for getpid(), getppid(), pause()
#include <signal.h>   // for signal handling
#include "eprintf.h"


/*
 * ASSIGNMENT
 *
 * - "#include" any other necessary headers (as indicated by "man"
 *    pages)
 */

 #ifndef NO_ARG
 enum {
     NO_ARG  = 0, // the option takes no argument
     REQ_ARG = 1, // the option must have an argument
     OPT_ARG = 2  // the option takes an optional argument
 };
 #endif

static struct option options[] = {
    { "children", OPT_ARG,  NULL,  'c'},
    { "help",     NO_ARG,   NULL,  'h'},
    { "nosync",   NO_ARG,   NULL,  'n'},
    { "pgid",     NO_ARG,   NULL,  'g'},
    { "ppid",     NO_ARG,   NULL,  'p'},
    { NULL, 0, NULL, 0 }  // end of options table
};

int showPpids = 0;
int showPgids = 0;
int synchronize = 1;

enum { IN_PARENT = -1 };
int siblingIndex = IN_PARENT;
int signalCount = 0;

void writeLog(char message[], const char *fromWithin)
{
    char processName[20];
    int colonIndent;

    if (siblingIndex == IN_PARENT) {
        strcpy(processName, "parent");
        colonIndent = 20;
    } else {
        snprintf(processName, sizeof(processName), "child #%d", siblingIndex);
        colonIndent = 30;
    }

    printf("%*s: %s\n", colonIndent, "process name", processName);
    printf("%*s: %d\n", colonIndent, "pid", getpid());
    
    if (showPpids) {
        printf("%*s: %d\n", colonIndent, "ppid", getppid());
    }
    if (showPgids) {
        printf("%*s: %d\n", colonIndent, "pgid", getpgrp());
    }
    printf("%*s: %d\n", colonIndent, "signalCount", signalCount);
    printf("%*s: %s\n", colonIndent, "message", message);
    printf("%*s: %s\n", colonIndent, "fromWithin", fromWithin);
    printf("\n");
}

void handler(int sigNum)
{
    signalCount++;
    char message[100];
    snprintf(message, sizeof(message), "Received signal %d: %s", sigNum, strsignal(sigNum));
    writeLog(message, __func__);

    if (sigNum == SIGTERM) {
        exit(0);  // Gracefully exit on SIGTERM
    }
}

void initSignals(void)
{
    for (int i = 1; i < NSIG; i++) {
        // Skip setting handlers and logging errors for SIGKILL and SIGSTOP
        if (i == SIGKILL || i == SIGSTOP) {
            continue;
        }

        if (signal(i, handler) == SIG_ERR) {
            char message[100];
            snprintf(message, sizeof(message), "Failed to set handler for signal %d: %s", i, strsignal(i));
            writeLog(message, __func__);
        }
    }
}


static void usage(char *progname)
{
    fprintf(stderr, "usage: %s [{args}]*\n", progname);
    fprintf(stderr, "%s\n", " {args} are:");
    fprintf(stderr, "%s",
        "  -g or --pgid                     list process group IDs\n"
        "  -n or --nosync                   turn off synchronization\n"
        "  -p or --ppid                     list parent PIDs (default: no)\n"
        "  -h or --help                     help (this message) and exit\n"
    );
}

int main(int argc, char **argv)
{
    int ch;
    static char *progname = "**UNSET**";

    progname = argv[0];
    for (;;) {
        ch = getopt_long(argc, argv, "c::ghnp", options, NULL);
        if (ch == -1)
            break;

        switch (ch) {
            case 'g':
                showPgids = 1;
                break;
            case 'h':
                usage(progname);
                exit(0);
            case 'n':
                synchronize = 0;
                break;
            case 'p':
                showPpids = 1;
                break;
            default:
                printf("?? getopt returned character code 0x%02x ??\n", ch);
                exit(1);
        }
    }

    initSignals();
    writeLog("Process is paused, waiting for a signal", __func__);
    while (1) {
        pause();
    }
    return 0;
}
