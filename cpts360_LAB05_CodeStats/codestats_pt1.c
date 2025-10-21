#include <stdio.h>
#include <assert.h> // for assert()

struct CodeStats {
    int lineCount;
};


void codeStats_init(struct CodeStats *codeStats)
{
    codeStats->lineCount = 0;
}


void codeStats_print(struct CodeStats codeStats, char *fileName)
{
    printf("           file: %s\n", fileName);
    printf("     line count: %d\n", codeStats.lineCount);
}


void codeStats_accumulate(struct CodeStats *codeStats, char *fileName)
{
    FILE *f = fopen(fileName, "r");
    int ch;
    enum {
        START,
    } state = START;

    assert(f);
    while ((ch = getc(f)) != EOF) {
        switch (state) {

        case START:
            if (ch == '\n') {
                codeStats->lineCount++;
            }
            break;

        default:
            assert(0);
            break;
        }
    }
    fclose(f);
    assert(state == START);
}


int main(int argc, char *argv[])
{
    struct CodeStats codeStats;
    int i;

    for (i = 1; i < argc; i++) {
        codeStats_init(&codeStats);
        codeStats_accumulate(&codeStats, argv[i]);
        codeStats_print(codeStats, argv[i]); // no "&" -- see why?
        if (i != argc-1)   // if this wasn't the last file ...
            printf("\n");  // ... print out a separating newline
    }
    return 0;
}
