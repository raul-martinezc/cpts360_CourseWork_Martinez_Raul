#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <limits.h>
#include "permissions.h"

// Sorting function for qsort
int compare_users(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

// Function to check if a user can perform a given action
int user_can_perform_action(struct stat *st, const char *action, struct passwd *user, const char *fsobj) {
    uid_t uid = user->pw_uid;
    gid_t gid = user->pw_gid;
    mode_t mode = st->st_mode;

    // Root always has access
    if (uid == 0) return 1;

    if (strcmp(action, "read") == 0) {
        return (uid == st->st_uid && (mode & S_IRUSR)) ||
               (gid == st->st_gid && (mode & S_IRGRP)) ||
               (mode & S_IROTH);
    } else if (strcmp(action, "write") == 0) {
        return (uid == st->st_uid && (mode & S_IWUSR)) ||
               (gid == st->st_gid && (mode & S_IWGRP)) ||
               (mode & S_IWOTH);
    } else if (strcmp(action, "execute") == 0) {
        return (uid == st->st_uid && (mode & S_IXUSR)) ||
               (gid == st->st_gid && (mode & S_IXGRP)) ||
               (mode & S_IXOTH);
    } else if (strcmp(action, "cd") == 0 || strcmp(action, "search") == 0) {
        return S_ISDIR(mode) &&
               ((uid == st->st_uid && (mode & S_IXUSR)) ||
                (gid == st->st_gid && (mode & S_IXGRP)) ||
                (mode & S_IXOTH));
    } else if (strcmp(action, "ls") == 0) {
        return S_ISDIR(mode) &&
               ((uid == st->st_uid && (mode & S_IRUSR)) ||
                (gid == st->st_gid && (mode & S_IRGRP)) ||
                (mode & S_IROTH));
    } else if (strcmp(action, "delete") == 0) {
        struct stat parent_stat;
        char parent_path[PATH_MAX];

        snprintf(parent_path, sizeof(parent_path), "%s/..", fsobj);
        if (stat(parent_path, &parent_stat) == -1) return 0;

        if (parent_stat.st_mode & S_ISVTX) {  // Sticky bit set
            return (uid == st->st_uid) || (uid == parent_stat.st_uid) || (uid == 0);
        } else {
            return (uid == st->st_uid && (parent_stat.st_mode & S_IWUSR)) ||
                   (gid == st->st_gid && (parent_stat.st_mode & S_IWGRP)) ||
                   (parent_stat.st_mode & S_IWOTH);
        }
    }

    return 0;
}

// Function to find users who can perform an action on fsobj
void find_users_who_can(const char *action, const char *fsobj) {
    struct stat st;
    struct passwd *pw;

    // Ensure PATH_MAX is defined
    #ifndef PATH_MAX
    #define PATH_MAX 4096
    #endif

    char resolved_path[PATH_MAX];

    if (realpath(fsobj, resolved_path) == NULL) {
        if (errno == EACCES) {
            fprintf(stderr, "Permission denied: Cannot access %s\n", fsobj);
        } else {
            perror("Error resolving path");
        }
        exit(EXIT_FAILURE);
    }

    if (stat(resolved_path, &st) == -1) {
        perror("Error getting file status");
        exit(EXIT_FAILURE);
    }

    // Start with a small allocation and grow dynamically
    int capacity = 100;
    char **user_list = malloc(capacity * sizeof(char *));
    if (!user_list) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    int user_count = 0;
    int canEveryone = 1;

    while ((pw = getpwent()) != NULL) {
        if (pw->pw_name == NULL) continue;  // Avoid NULL pointer access

        if (user_can_perform_action(&st, action, pw, resolved_path)) {
            // Check for duplicates before adding
            int is_duplicate = 0;
            for (int i = 0; i < user_count; i++) {
                if (strcmp(user_list[i], pw->pw_name) == 0) {
                    is_duplicate = 1;
                    break;
                }
            }

            if (!is_duplicate) {
                // Expand memory dynamically if needed
                if (user_count >= capacity) {
                    capacity *= 2;  // Double capacity
                    char **new_list = realloc(user_list, capacity * sizeof(char *));
                    if (!new_list) {
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(EXIT_FAILURE);
                    }
                    user_list = new_list;
                }

                user_list[user_count] = strdup(pw->pw_name);  // Copy username
                if (!user_list[user_count]) {
                    fprintf(stderr, "Memory allocation failed\n");
                    exit(EXIT_FAILURE);
                }
                user_count++;
            }
        } else {
            canEveryone = 0;
        }
    }
    endpwent();

    if (user_count == 0) {
        free(user_list);
        exit(EXIT_SUCCESS);
    }

    // If every user in the system has access, print "(everyone)"
    if (canEveryone) {
        printf("(everyone)\n");
    } else {
        // Sort unique usernames
        qsort(user_list, user_count, sizeof(char *), compare_users);

        for (int i = 0; i < user_count; i++) {
            printf("%s\n", user_list[i]);
            free(user_list[i]);  // Free each allocated username
        }
    }

    free(user_list);  // Free the list itself
}
//qwerty