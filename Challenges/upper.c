/*
*   Dylan Oldfield
*
*   This program turns all alphabetical characters upper case
*/
#include <stdio.h>
#include <ctype.h>

int
main(void) {
    int c;
    
    c = getchar();
    while (c != EOF) {

        if (c >= 'a' && c <= 'z') {
           c = toupper(c);
        }

        putchar(c);

        c = getchar();
    }
    return 0;
}