/*

Dylan Oldfield 

This Program reverses a stdin input line by line
reversing each line seperately.
*/

#include <stdio.h>
#include <string.h> 
int main(void)
{
    char phrase[101];
   while(fgets(phrase, 101, stdin) != NULL)
   {
    int n = strlen(phrase);
    char c = phrase[n-1];
    for(int i = n - 2; i != -1; i --)
    {
        c = phrase[i];
        putchar(c);
    }
    putchar('\n');
       
   }
    
}