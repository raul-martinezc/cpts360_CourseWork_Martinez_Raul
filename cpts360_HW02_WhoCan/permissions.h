#ifndef PERMISSIONS_H
#define PERMISSIONS_H

#include <sys/stat.h>
#include <pwd.h>

// Function prototypes
int user_can_perform_action(struct stat *st, const char *action, struct passwd *user, const char *fsobj);
void find_users_who_can(const char *action, const char *fsobj);
int compare_users(const void *a, const void *b);  // Declare compare_users

#endif
