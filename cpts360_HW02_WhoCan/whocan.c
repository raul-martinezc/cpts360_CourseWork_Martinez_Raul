#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "permissions.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <action> <fsobj>\n", argv[0]);
        return EXIT_FAILURE;
    }

    find_users_who_can(argv[1], argv[2]);

    return EXIT_SUCCESS;
}
