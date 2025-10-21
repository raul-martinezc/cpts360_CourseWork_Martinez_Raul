#ifndef SHOW_CPP_ONLY // this is for documentation: the #includes are required
#include <stdlib.h>
#include <stdio.h>
#endif

#include "except.h"

Except_T NoSuchFile = { "Failed to open file." };

void doFile(char *fn)
{
    FILE *f = fopen(fn, "r");
    if (!f) {
        perror("Error opening file");
        RAISE(NoSuchFile);
    } else {
        printf("Successfully opened: %s\n", fn);
    }
    fclose(f);
}


int main(int argc, char *argv[])
{
    int i;
    int ctAttempted = 0, ctFail = 0;

    for (i = 1; i < argc; i++) {
        ctAttempted++;
        TRY
            doFile(argv[i]);
        EXCEPT(NoSuchFile)
            ctFail++;
        END_TRY;
    }
    printf("%d open(s) attempted, %d failed\n", ctAttempted, ctFail);
    exit(EXIT_SUCCESS);
}
