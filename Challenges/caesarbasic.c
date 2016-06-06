
/**
*caeser.c
*
*Dylan P Oldfield
*dylan.oldfield@yahoo.com
*
*This programs encrypts input  data based on an integer key k
*provided by the user. It utilises caeser cipher which transposes
*letter c by k overall with 26 being the limit
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(void)
{
    // gets phrase to encrypt from user
    char buffer[100];
    int key;
    printf("Enter key: ");
    fgets(buffer,100, stdin);
    
    if (sscanf(buffer,"%d", &key) != 1)
    {
        printf("\nInvalid key!\n");
        return 0;
    }
    
    if (key < 0 || key > 26)
    {
        printf("\nInvalid key!\n");
        return 0;
    }
    printf("Enter line: ");
    
    char phrase[100];
    fgets(phrase, 100, stdin);
    int length = strlen(phrase);
    printf("\n");
    // goes through string checking to see if characters are alphabetical
    for(int i=0; i < length-1; i++)
    {
        char c = phrase[i];
       // wrap-around for lower-case characters
        if ( islower(c) )
        {
            printf("%c",(97 + ((c + key) % 97) % 26));
        }
       // wrap-around for upper-case characters
        else if ( isupper(phrase[i]) )
        {
            printf("%c",(65 + ( ( c + key ) % 65) % 26));
        }         
        else
        {
            printf("%c", c);
        }     
       
    }
    printf("\n");
    return 0;
    
}