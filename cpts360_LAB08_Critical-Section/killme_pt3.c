#include <stdlib.h>    // for exit(), atoi()
#include <stdio.h>     // for printf(), perror()
#include <getopt.h>    // for getopt()
#include <string.h>    // for strcpy(), snprintf(), strsignal()
#include <unistd.h>    // for fork(), getpid(), getppid(), pause()
#include <signal.h>    // for signal handling
#include <sys/types.h> // for pid_t
#include <sys/wait.h>  // for wait()
#include "critical_section.h" // For synchronization

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

/*
 * Command-line options table
 */
static struct option options[] = {
   { "children", OPT_ARG,  NULL,  'c'},
   { "help",     NO_ARG,   NULL,  'h'},
   { "nosync",   NO_ARG,   NULL,  'n'},
   { "pgid",     NO_ARG,   NULL,  'g'},
   { "ppid",     NO_ARG,   NULL,  'p'},
   { NULL, 0, NULL, 0 }  // End of options table
};

/*
 * These globals are set by command-line options. Here, they are set
 * to their default values.
 */
int showPpids = 0;   // Show parent process IDs
int showPgids = 0;   // Show process group IDs
int synchronize = 1; // Synchronize outputs

enum { IN_PARENT = -1 }; // Must be negative
/*
 * In the parent, this value is IN_PARENT. In the children, it's set
 * to the order in which they were spawned, starting at 0.
 */
int siblingIndex = IN_PARENT;

// This is a global count of signals received.
int signalCount = 0;

/*
 * Print identifying information about the current process to stdout.
 * If synchronization is enabled, enter and leave the critical section.
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

    if (synchronize) {
        criticalSection_enter();
    }

    printf("------------------------------------------------\n");
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
    printf("------------------------------------------------\n\n");
    fflush(stdout);

    if (synchronize) {
        criticalSection_leave();
    }
}

/*
 * Do everything that's supposed to be done in the child.
 * - Set the global `siblingIndex` to `iSibling`.
 * - Print an entry to the log that the process is paused.
 * - Wait indefinitely for signals.
 */
void inChild(int iSibling) {
    siblingIndex = iSibling;
    writeLog("Child process is paused, waiting for signals", __func__);

    while (1) {
        pause();
    }
}

/*
 * Handle signal `sigNum`
 * - Increment signalCount.
 * - Create a message that includes `sigNum` and its string equivalent.
 * - Add an entry to the log that includes that message.
 * - If the signal is SIGTERM, terminate the process.
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
 * Initialize all signals.
 * - For every signal from 1 to NSIG (except SIGTRAP and SIGQUIT),
 *   set the handler to `handler()`. If setting the handler fails,
 *   log a message.
 */
void initSignals(void) {
    for (int i = 1; i < NSIG; i++) {
        // Ensure the signal is within a valid range
        if (i == SIGKILL || i == SIGSTOP || i == SIGTRAP || i == SIGQUIT) {
            continue;  // Skip uncatchable signals
        }

        // Skip invalid signal numbers (MacOS might have different ones)
        if (i >= NSIG) {
            break; // Avoid accessing out-of-range signals
        }

        if (signal(i, handler) == SIG_ERR) {
            char message[100];
            snprintf(message, sizeof(message), "Failed to set handler for signal %d: %s", i, strsignal(i));
            writeLog(message, __func__);
        }
    }
}


/*
 * Do everything that's supposed to be done in the parent.
 * - Log that the parent is waiting for child processes.
 * - Wait for all child processes to exit.
 * - Log each child's exit status.
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
 * Issue a usage error message.
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
 * Main function.
 * - Parse command-line arguments.
 * - If synchronization is enabled, initialize the critical section.
 * - Initialize signals.
 * - If no children are spawned, pause indefinitely for a signal.
 * - Otherwise, fork `nSiblings` children.
 * - Each child calls `inChild()`.
 * - The parent waits for all children in `inParent()`.
 */
int main(int argc, char **argv) {
    int ch;
    int nSiblings = 0;
    static char *progname = "**UNSET**";

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

    if (synchronize) {
        criticalSection_init();
    }

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
