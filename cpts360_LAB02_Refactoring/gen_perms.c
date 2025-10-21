//
//  gen_perms.c
//  lab_refactoring_Raul_Martinez
//
//  Created by Raul Martinez on 1/14/25.
//

#include <stdlib.h>
#include "gen_perms.h"

//Helper recursive function to generate permutations
static void generate(int *perm, int *used, int level, int nElems,
                     void (*handlePerm)(int elems[], int nElems, void *userArg),
                     void *userArg) {
    if (level == nElems) {
        handlePerm(perm, nElems, userArg);
        return;
    }

    for (int i = 0; i < nElems; i++) {
        if (!used[i]) {
            perm[level] = i;
            used[i] = 1;
            generate(perm, used, level + 1, nElems, handlePerm, userArg);
            used[i] = 0;  // Backtrack
        }
    }
}

void genPerms(int nElems,
              void (*handlePerm)(int elems[], int nElems, void *userArg),
              void *userArg) {
    int *perm = malloc(nElems * sizeof(int));  // Array to store current permutation
    int *used = calloc(nElems, sizeof(int));   // Track used elements

    generate(perm, used, 0, nElems, handlePerm, userArg);

    free(perm);
    free(used);
}
