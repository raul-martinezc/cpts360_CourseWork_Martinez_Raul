// It's not a bad practice to list *why* you include particular
// headers.
#include <stdio.h> // for FILE, NULL, fopen(), and getc()

#include "eprintf.h" // for eprintf_fail()

// Although not strictly required, its a good practice to include the
// header file that corresponds to this source file to detect any
// discrepancies between the function's declaration and its
// definition.
#include "compare_files.h"


int compareFiles(char *fname0, char *fname1)
{
    //
    // ASSIGNMENT
    //
    
    // This function compares two files named `fname0` and `fname1`
    // and returns true (1) if they are identical or false (0) if they
    // are not. Here's how it would be described in pseudocode (note
    // the indented block structure):
    
    FILE *file0, *file1;
    int ch0, ch1;

    file0 = fopen(fname0, "r");         //   open file 0 for reading (hint: fopen(3))
    if (file0 == NULL) {                // if the open fails,
        eprintf_fail("Failed to open file: %s", fname0);    //exit with an error message
    }

    file1 = fopen(fname1, "r");         //open file 1 for reading (hint: fopen(3))
    if (file1 == NULL) {                //if the open fails,
        fclose(file0);  // Close the first file before exiting
        eprintf_fail("Failed to open file: %s", fname1);    //exit with an error message
    }

    while (1) {                         //repeat until this function returns:
        ch0 = getc(file0);              //read a character `ch0` from file 0 (hint: getc(3))
        ch1 = getc(file1);              //read a character `ch1` from file 1 (hint: getc(3))

        if (ch0 != ch1) {               //compare both characters to each other and to `EOF`, (possibly) returning 0 or 1
            fclose(file0);
            fclose(file1);
            return 0;  // Files are different
        }

        if (ch0 == EOF) {               //compare both characters to each other and to `EOF`, (possibly) returning 0 or 1
            fclose(file0);
            fclose(file1);
            return 1;  // Files are identical (both reached EOF)
            
        }
    }
}
    //***********************INSTRUCTIONS****************************

    //
    // ASSIGNMENT
    //
    // This function compares two files named `fname0` and `fname1`
    // and returns true (1) if they are identical or false (0) if they
    // are not. Here's how it would be described in pseudocode (note
    // the indented block structure):
    //   open file 0 for reading (hint: fopen(3))
    //   if the open fails,
    //       exit with an error message
    //   open file 1 for reading (hint: fopen(3))
    //   if the open fails,
    //       exit with an error message
    //   repeat until this function returns:
    //       read a character `ch0` from file 0 (hint: getc(3))
    //       read a character `ch1` from file 1 (hint: getc(3))
    //       compare both characters to each other and to `EOF`,
    //        (possibly) returning 0 or 1
    //
    // The last statement is intentionally vague. The logic here is
    // important. No extra points challenge: It can be done in a
    // single `if`-block.
    //
 
    
        
            
       
