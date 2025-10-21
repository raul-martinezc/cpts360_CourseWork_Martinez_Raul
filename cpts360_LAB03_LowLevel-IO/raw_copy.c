#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "syscall_check.h"
#include "allocarray.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s bufferSize inputFilename outputFilename\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Step 1: Parse buffer size
    int bufferSize = atoi(argv[1]);
    if (bufferSize <= 0) {
        fprintf(stderr, "Error: bufferSize must be a positive integer\n");
        exit(EXIT_FAILURE);
    }

    // Step 2: Allocate buffer
    char *buffer;
    ALLOC_ARRAY(buffer, char, bufferSize);

    // Step 3: Open input file
    int inputFd;
    SYSCALL_CHECK(inputFd = open(argv[2], O_RDONLY));

    // Step 4: Open output file
    int outputFd;
    SYSCALL_CHECK(outputFd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644));

    // Step 5: Copy data
    ssize_t bytesRead, bytesWritten;
    while ((bytesRead = read(inputFd, buffer, bufferSize)) > 0) {
        SYSCALL_CHECK(bytesWritten = write(outputFd, buffer, bytesRead));
        if (bytesWritten != bytesRead) {
            fprintf(stderr, "Error: Partial write occurred\n");
            exit(EXIT_FAILURE);
        }
    }
    if (bytesRead < 0) {
        perror("Error reading from input file");
        exit(EXIT_FAILURE);
    }

    // Step 6: Clean up
    FREE_ARRAY(buffer);
    SYSCALL_CHECK(close(inputFd));
    SYSCALL_CHECK(close(outputFd));

    return 0;
}
