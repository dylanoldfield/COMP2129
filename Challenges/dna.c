#include <stdio.h>
#include <string.h> 

int main (void)
{
    printf("Enter strand: ");
    char string[100];
    if(scanf("%s", string) != 1)
    {
        printf("\n\nNo strand provided.\n");
        return 0;
    }
    
    int n = strlen(string);
    for(int i = 0; i < n; i++)
    {
        char c = string[i];
        if(strchr("aA",c) != NULL)
        {
            c = c + 19;
            string[i] = c;
        }
        else if (strchr("tT",c) != NULL)
        {
            c = c - 19;
            string[i] = c;
        }
        else if (strchr("gG",c) != NULL)
        {
            c = c - 4;
            string[i] = c;
        }
        else if (strchr("cC",c) != NULL)
        {
            c = c + 4;
            string[i] = c;
        }
        else
        {
            string[i] = 'x';
        }
        
    }
    printf("\n\nComplementary strand is %s\n", string);
    return 0;
}