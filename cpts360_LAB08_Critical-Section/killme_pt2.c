#include <stdlib.h>    // for exit(), atoi()
#include <stdio.h>     // for printf(), perror()
#include <getopt.h>    // for getopt()
#include <string.h>    // for strcpy(), snprintf(), strsignal()
#include <unistd.h>    // for fork(), getpid(), getppid(), pause()
#include <signal.h>    // for signal handling
#include <sys/types.h> // for pid_t
#include <sys/wait.h>  // for wait()

/*
 * ASSIGNMENT
 *
 * - "#include" any other necessary headers (as indicated by "man"
 *    pages)
 */

// Option values for getopt_long()
#ifndef NO_ARG
enum {
    NO_ARG  = 0, // the option takes no argument
    REQ_ARG = 1, // the option must have an argument
    OPT_ARG = 2  // the option takes an optional argument
};
#endif

// Command-line options table
static struct option options[] = {
   { "children", OPT_ARG,  NULL,  'c'},
   { "help",     NO_ARG,   NULL,  'h'},
   { "nosync",   NO_ARG,   NULL,  'n'},
   { "pgid",     NO_ARG,   NULL,  'g'},
   { "ppid",     NO_ARG,   NULL,  'p'},
   { NULL, 0, NULL, 0 }  // end of options table
};

/*
 * These globals are set by command line options. Here, they are set
 * to their default values.
 */
int showPpids = 0;   // Show parent process IDs
int showPgids = 0;   // Show process group IDs
int synchronize = 1; // Synchronize outputs (for later parts)

// Process identification
enum { IN_PARENT = -1 };
int siblingIndex = IN_PARENT; // -1 for parent, >=0 for children

// Signal handling count
int signalCount = 0;

/*
 * Print log messages directly to the console
 */
void writeLog(char message[], const char *fromWithin) {
    char processName[20];
    int colonIndent;

    if (siblingIndex == IN_PARENT) {
        strcpy(processName, "parent");
        colonIndent = 20;
    } else {
        snprintf(processName, sizeof(processName), "child #%d", siblingIndex);
        colonIndent = 30;
    }

    // Print formatted output with separators
    printf("\n------------------------------------------------\n");
    printf("%*s: %s\n", colonIndent, "Process", processName);
    printf("%*s: %d\n", colonIndent, "PID", getpid());

    if (showPpids) {
        printf("%*s: %d\n", colonIndent, "PPID", getppid());
    }
    if (showPgids) {
        printf("%*s: %d\n", colonIndent, "PGID", getpgrp());
    }

    printf("%*s: %d\n", colonIndent, "Signal Count", signalCount);
    printf("%*s: %s\n", colonIndent, "Message", message);
    printf("%*s: %s\n", colonIndent, "From", fromWithin);
    printf("------------------------------------------------\n");

    fflush(stdout); // Force output order to prevent mixing logs
}

/*
 * Child process function
 */
void inChild(int iSibling) {
    siblingIndex = iSibling;
    writeLog("Child process is paused, waiting for signals", __func__);

    while (1) {
        pause(); // Wait for signals indefinitely
    }
}

/*
 * Signal handler function
 */
void handler(int sigNum) {
    signalCount++;
    char message[100];
    snprintf(message, sizeof(message), "Received signal %d: %s", sigNum, strsignal(sigNum));
    writeLog(message, __func__);

    if (sigNum == SIGTERM) {
        writeLog("Process terminating", __func__);
        exit(0);
    }
}

/*
 * Initialize signal handlers for all catchable signals
 */
void initSignals(void) {
    for (int i = 1; i < NSIG; i++) {
        if (i == SIGKILL || i == SIGSTOP || i == SIGTRAP || i == SIGQUIT) {
            continue; // Skip uncatchable signals
        }

        if (signal(i, handler) == SIG_ERR) {
            char message[100];
            snprintf(message, sizeof(message), "Failed to set handler for signal %d: %s", i, strsignal(i));
            writeLog(message, __func__);
        }
    }
}

/*
 * Parent process function: Wait for child processes to exit
 */
void inParent(void) {
    writeLog("Parent is waiting for child processes to exit", __func__);

    int status;
    pid_t childPid;
    while ((childPid = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            char message[100];
            snprintf(message, sizeof(message), "Child %d exited normally with status %d", childPid, WEXITSTATUS(status));
            writeLog(message, __func__);
        } else if (WIFSIGNALED(status)) {
            char message[100];
            snprintf(message, sizeof(message), "Child %d terminated by signal %d: %s",
                     childPid, WTERMSIG(status), strsignal(WTERMSIG(status)));
            writeLog(message, __func__);
        } else {
            char message[100];
            snprintf(message, sizeof(message), "Child %d exited abnormally", childPid);
            writeLog(message, __func__);
        }
    }
}

/*
 * Display usage information
 */
static void usage(char *progname) {
    fprintf(stderr, "usage: %s [{args}]*\n", progname);
    fprintf(stderr, "%s\n", " {args} are:");
    fprintf(stderr, "%s",
        "  -c[{arg}] or --children[={arg}]  fork {arg} children (default: 1)\n"
        "  -g or --pgid                     list process group IDs\n"
        "  -n or --nosync                   turn off synchronization\n"
        "  -p or --ppid                     list parent PIDs (default: no)\n"
        "  -h or --help                     help (this message) and exit\n"
    );
}

/*
 * Main function
 */
int main(int argc, char **argv) {
    int ch;
    int nSiblings = 0;
    static char *progname = "**UNSET**";

    /*
     * Parse command-line arguments.
     */
    progname = argv[0];
    for (;;) {
        ch = getopt_long(argc, argv, "c::ghnp", options, NULL);
        if (ch == -1) break;

        switch (ch) {
            case 'c':
                if (optarg) nSiblings = atoi(optarg);
                else nSiblings = 1;
                break;
            case 'g': showPgids = 1; break;
            case 'h': usage(progname); exit(0);
            case 'n': synchronize = 0; break;
            case 'p': showPpids = 1; break;
            default:
                printf("?? getopt returned character code 0x%02x ??\n", ch);
                exit(1);
        }
    }

    /*
     * Initialize signals and either:
     * - Pause if no children are spawned
     * - Fork children and wait for them in the parent
     */
    initSignals(); 

    if (nSiblings == 0) {
        writeLog("Parent process is paused, waiting for a signal", __func__);
        while (1) {
            pause();
        }
    } else {
        for (int i = 0; i < nSiblings; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                inChild(i); 
                exit(0);
            } else if (pid > 0) {
                char message[100];
                snprintf(message, sizeof(message), "Forked child %d with PID %d", i, pid);
                writeLog(message, __func__);
            } else {
                perror("fork");
                exit(1);
            }
        }
        inParent(); 
    }

    return 0;
}
