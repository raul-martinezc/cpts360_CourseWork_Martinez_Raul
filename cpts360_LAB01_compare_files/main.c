#include <stdio.h>
#include "compare_files.h"
#include "eprintf.h"


int main(int argc, char *argv[])
{
    if (argc != 3)
        eprintf_fail("syntax: %s {file0} {file1}\n", argv[0]);
    if (compareFiles(argv[1], argv[2]))
        printf("files are identical\n");
    else
        printf("files differ\n");
    return 0;
}
