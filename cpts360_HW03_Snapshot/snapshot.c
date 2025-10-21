// snapshot.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <sys/resource.h>
#include <dirent.h>

#include "snapshot.h"

extern int snapshot_debug;

static int copy_file(const char *src, const char *dst) {
    int in_fd = open(src, O_RDONLY);
    if (in_fd < 0) return -1;

    int out_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (out_fd < 0) {
        close(in_fd);
        return -1;
    }

    char buf[4096];
    ssize_t n;
    while ((n = read(in_fd, buf, sizeof(buf))) > 0) {
        if (write(out_fd, buf, n) != n) {
            close(in_fd);
            close(out_fd);
            return -1;
        }
    }

    close(in_fd);
    close(out_fd);
    return 0;
}

int snapshot(char *ssname, char *progpath, char *readme) {
    struct stat st;

    // Step 1: Check if ssname dir exists
    if (stat(ssname, &st) == 0) {
        if (snapshot_debug) fprintf(stderr, "Directory %s already exists\n", ssname);
        return -1;
    }

    // Step 2: Create ssname dir
    if (mkdir(ssname, 0755) < 0) {
        if (snapshot_debug) perror("mkdir");
        return -1;
    }

    // Step 3: Enable unlimited core dumps
    struct rlimit rl;
    rl.rlim_cur = RLIM_INFINITY;
    rl.rlim_max = RLIM_INFINITY;
    if (setrlimit(RLIMIT_CORE, &rl) < 0) {
        if (snapshot_debug) perror("setrlimit");
        return -1;
    }

    // Step 4: Fork to generate core dump
    pid_t child = fork();
    if (child == -1) {
        if (snapshot_debug) perror("fork");
        return -1;
    }
    if (child == 0) {
        abort();
    }

    // Wait for core dump
    int status;
    waitpid(child, &status, 0);

    // Step 5: Rename core dump to core.<parent_pid>
    char corefile[64], newcore[PATH_MAX];
    snprintf(corefile, sizeof(corefile), "core.%d", child);
    snprintf(newcore, sizeof(newcore), "%s/core.%d", ssname, getpid());
    if (rename(corefile, newcore) < 0) {
        if (snapshot_debug) perror("rename core");
        return -1;
    }

    // Step 6: Copy executable from /proc/self/exe
    char exe_link[PATH_MAX], exe_target[PATH_MAX];
    snprintf(exe_link, sizeof(exe_link), "/proc/%d/exe", getpid());
    ssize_t len = readlink(exe_link, exe_target, sizeof(exe_target) - 1);
    if (len < 0) {
        if (snapshot_debug) perror("readlink");
        return -1;
    }
    exe_target[len] = '\0';

    char progpath_copy[PATH_MAX];
    strncpy(progpath_copy, progpath, sizeof(progpath_copy));
    progpath_copy[sizeof(progpath_copy) - 1] = '\0';
    char *progname = basename(progpath_copy);

    char out_exec[PATH_MAX];
    snprintf(out_exec, sizeof(out_exec), "%s/%s", ssname, progname);

    if (copy_file(exe_target, out_exec) < 0) {
        if (snapshot_debug) perror("copy_file");
        return -1;
    }

    // Step 7: Write README.txt
    char readmefile[PATH_MAX];
    snprintf(readmefile, sizeof(readmefile), "%s/README.txt", ssname);
    FILE *rf = fopen(readmefile, "w");
    if (!rf) {
        if (snapshot_debug) perror("fopen README");
        return -1;
    }
    fprintf(rf, "%s%s", readme, (readme[strlen(readme)-1] == '\n') ? "" : "\n");
    fclose(rf);

    // Step 8: Make tarball
    char tar_cmd[PATH_MAX * 2];
    snprintf(tar_cmd, sizeof(tar_cmd), "tar czf %s.tgz %s", ssname, ssname);
    if (system(tar_cmd) != 0) {
        if (snapshot_debug) fprintf(stderr, "tar command failed\n");
        return -1;
    }

    // Step 9: Remove ssname directory and contents
    unlink(out_exec);
    unlink(newcore);
    unlink(readmefile);

    DIR *d = opendir(ssname);
    if (d) {
        struct dirent *entry;
        char filepath[PATH_MAX];
        while ((entry = readdir(d)) != NULL) {
            if (strncmp(entry->d_name, "core.", 5) == 0) {
                snprintf(filepath, sizeof(filepath), "%s/%s", ssname, entry->d_name);
                unlink(filepath);
            }
        }
        closedir(d);
    }
    rmdir(ssname);

    return 0;
}
