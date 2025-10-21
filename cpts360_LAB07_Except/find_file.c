#define _GNU_SOURCE // to get asprintf() prototype
#include <stdio.h>  // this needs to be the first #include in that case

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <getopt.h>

#include "except.h"

/* Exception Declarations */
Except_T StatFailed = { "Failed to open file." };
Except_T TargetFound = { "Target file found." };
Except_T MiscellaneousError = { "Miscellaneous error occurred." };

char *progname = "*** progname not set ***"; /* should be argv[0] */
int verbose = 0; /* set on command line */

static void explore(char *path, char *target);

/* Traverses a directory and searches for the target file */
static void traverseDirectory(char path[], char *target)
{
    DIR *dir = opendir(path);
    if (!dir) {
        RAISE(MiscellaneousError);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char *subpath;
        asprintf(&subpath, "%s/%s", path, entry->d_name);

        TRY {
            explore(subpath, target);
        } 
        EXCEPT(TargetFound) {
            free(subpath);
            closedir(dir);
            RERAISE;
        } 
        ELSE {
            fprintf(stderr, "explore() failed on: %s\n", subpath);
        } 
        END_TRY;  

        free(subpath);
    } 
    closedir(dir);
}

/* Recursively explores the given path looking for the target file */
static void explore(char *path, char *target)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        RAISE(StatFailed);
    }

    // Extract the last component (filename)
    char *filename = strrchr(path, '/');
    if (filename) {
        filename++; // Move past the '/'
    } else {
        filename = path;
    }

    // Check if the filename matches the target
    if (strcmp(filename, target) == 0) {
        if (verbose) {
            printf("%s\n", path);
        }
        RAISE(TargetFound);
    }

    // If it's a directory, recursively explore it
    if (S_ISDIR(path_stat.st_mode)) {
        traverseDirectory(path, target);
    }
}

/* Starts the search for the file */
void findFile(char *top, char *target)
{
    TRY {
        explore(top, target);
    } 
    EXCEPT(StatFailed) {
        ; // Do nothing
    } 
    EXCEPT(TargetFound) {
        exit(EXIT_SUCCESS);
    }
    END_TRY;  
}

void usage(void)
{
    printf("usage: %s [-h] [-v] target [directory]*\n", progname);
}

int main(int argc, char *argv[])
{
    int i, ch;
    char *target;
    extern int optind;

    progname = argv[0];

    while ((ch = getopt(argc, argv, "hv")) != -1) {
        switch (ch) {
        case 'v':
            verbose = 1;
            break;
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        case '?':
            usage();
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        usage();
        exit(EXIT_FAILURE);
    }

    target = argv[optind++];
    if (optind == argc) {
        /* directory name(s) not provided */
        findFile(".", target);
    } else {
        /* directory name(s) provided */
        for (i = optind; i < argc; i++)
            findFile(argv[i], target);
    }

    /* If we find the target, we'll exit immediately */
    exit(EXIT_FAILURE);
}
