/*
*   Dylan Oldfield
*
*   This program reverses characters from std input completely 
*   the first line will be return reversed last. 
*/
#include <stdio.h>
#include <string.h> 
int main(void)
{
    int c; 
    c = getchar();
    int n = 0;
    char phrase[101];
    
    while( c != EOF)
    {
       phrase[n] = c;
       n++;
       c = getchar();
    }
    
    
    for(int i = n - 1; i != -1; i --)
    {
        c = phrase[i];
        putchar(c);
    }
}