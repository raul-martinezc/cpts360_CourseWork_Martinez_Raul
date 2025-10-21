#include <stdio.h>
#include <assert.h>
#include <ctype.h> // for isspace()

struct CodeStats {
    int lineCount;
    int linesWithCodeCount;
    int cplusplusCommentCount;
    int cCommentCount;
};

void codeStats_init(struct CodeStats *codeStats)
{
    codeStats->lineCount = 0;
    codeStats->linesWithCodeCount = 0;
    codeStats->cplusplusCommentCount = 0;
    codeStats->cCommentCount = 0;
}

void codeStats_print(struct CodeStats codeStats, char *fileName)
{
    printf("           file: %s\n", fileName);
    printf("     line count: %d\n", codeStats.lineCount);
    printf(" lines with code: %d\n", codeStats.linesWithCodeCount);
    printf("  C++ comments: %d\n", codeStats.cplusplusCommentCount);
    printf("    C comments: %d\n", codeStats.cCommentCount);
}

void codeStats_accumulate(struct CodeStats *codeStats, char *fileName)
{
    FILE *f = fopen(fileName, "r");
    int ch;
    int foundCodeOnLine = 0; // Flag to track non-whitespace chars
    enum {
        START,
        SLASH,
        CPP_COMMENT,
        C_COMMENT,
        C_COMMENT_END
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
                } else if (ch == '*') {
                    state = C_COMMENT;
                    codeStats->cCommentCount++; // Found C-style comment
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

            case C_COMMENT:
                if (ch == '*') {
                    state = C_COMMENT_END;  // Possible end of comment
                } else if (ch == '\n') {
                    codeStats->lineCount++;  // Count lines inside C comments
                } else {
                    // Stay in C_COMMENT until we find '*/'
                    state = C_COMMENT;
                }
                break;

            case C_COMMENT_END:
                if (ch == '/') {
                    state = START; // End of a C-style comment
                } else if (ch == '*') {
                    // Stay in C_COMMENT_END if another * is found
                    state = C_COMMENT_END;
                } else if (ch == '\n') {
                    codeStats->lineCount++; // Keep counting new lines inside comment
                    state = C_COMMENT; // Still inside comment
                } else {
                    state = C_COMMENT; // Go back to inside the comment
                }
                break;

            default:
                assert(0);
                break;
        }
    }
    fclose(f);
}
/*
Test code
*/
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
