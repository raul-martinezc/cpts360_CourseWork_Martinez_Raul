#include <stdio.h>

int level;
enum {
    N_ELEM = 3,
    NOT_DONE = -1
};
int val[N_ELEM];


void recur(int k)
{
    int i;

    val[k] = level;
    level++;
    if (level == N_ELEM) {
        for (i = 0; i < N_ELEM; i++)
            printf("%d ", val[i]);
        printf("\n");
    }
    for (i = 0; i < N_ELEM; i++)
        if (val[i] == NOT_DONE)
            recur(i);
    level--;
    val[k] = NOT_DONE;
}

int main(int argc, char *argv[])
{
    int i;

    level = 0;
    for (i = 0; i < N_ELEM; i++)
        val[i] = NOT_DONE;
    for (i = 0; i < N_ELEM; i++)
        recur(i);
    return 0;
}
