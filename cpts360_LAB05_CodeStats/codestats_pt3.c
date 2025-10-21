#include <stdio.h>
#include <assert.h>
#include <ctype.h> // for isspace()

struct CodeStats {
    int lineCount;
    int linesWithCodeCount;
    int cplusplusCommentCount;
};

void codeStats_init(struct CodeStats *codeStats)
{
    codeStats->lineCount = 0;
    codeStats->linesWithCodeCount = 0;
    codeStats->cplusplusCommentCount = 0;
}

void codeStats_print(struct CodeStats codeStats, char *fileName)
{
    printf("           file: %s\n", fileName);
    printf("     line count: %d\n", codeStats.lineCount);
    printf(" lines with code: %d\n", codeStats.linesWithCodeCount);
    printf("  C++ comments: %d\n", codeStats.cplusplusCommentCount);
}

void codeStats_accumulate(struct CodeStats *codeStats, char *fileName)
{
    FILE *f = fopen(fileName, "r");
    int ch;
    int foundCodeOnLine = 0; // Flag to track non-whitespace chars
    enum {
        START,
        SLASH,
        CPP_COMMENT
    } state = START;

    assert(f);
    while ((ch = getc(f)) != EOF) {
        switch (state) {
            case START:
                if (ch == '\n') {
                    codeStats->lineCount++;
                    if (foundCodeOnLine) {
                        codeStats->linesWithCodeCount++;
                    }
                    foundCodeOnLine = 0; // Reset flag for the next line
                } else if (ch == '/') {
                    state = SLASH; // Potential start of a comment
                } else if (!isspace(ch)) { 
                    foundCodeOnLine = 1; // Found non-whitespace character
                }
                break;

            case SLASH:
                if (ch == '/') {
                    state = CPP_COMMENT;
                    codeStats->cplusplusCommentCount++; // Found C++ comment
                } else {
                    state = START;
                    if (ch == '\n') {
                        codeStats->lineCount++;
                        if (foundCodeOnLine) {
                            codeStats->linesWithCodeCount++;
                        }
                        foundCodeOnLine = 0; // Reset flag for the next line
                    } else if (!isspace(ch)) {
                        foundCodeOnLine = 1;
                    }
                }
                break;

            case CPP_COMMENT:
                if (ch == '\n') {
                    state = START;
                    codeStats->lineCount++;
                    foundCodeOnLine = 0; // Reset since it's a comment
                }
                break;

            default:
                assert(0);
                break;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[])
{
    struct CodeStats codeStats;
    int i;

    for (i = 1; i < argc; i++) {
        codeStats_init(&codeStats);
        codeStats_accumulate(&codeStats, argv[i]);
        codeStats_print(codeStats, argv[i]);
        if (i != argc-1)   // Separate output for multiple files
            printf("\n");
    }
    return 0;
}
