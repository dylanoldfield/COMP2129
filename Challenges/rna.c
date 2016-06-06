/*
*
* Dylan P Oldfield  
* 440310604
* University of Sydney
*
* RNA spliceosome
*
*This program searches for introns in a strand of rna booked marked by "GUGU" 
* and the next "AGAG". It removes this intron from the RNA string provided 
* and returns the string without introns
*
* -String manipulation
* - Recursion
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h> 

//global variables: holding final string, length and pointer
char global[100];
char* gpt = global;
int glen = 0;

void rna(const char* string);

int main (void)
{
    // checks for correct input
    printf("Enter strand: ");
    char string[100];
    if(scanf("%s", string) != 1)
    {
        printf("\n\nNo strand provided.\n");
        return 0;
    }
    
    const char* argu = string;
    rna(argu);
    
    printf("\nOutput is %s\n", global);
}

void rna(const char* string)
{
        
    // checks for the "GUGU" bookmark
    char* gugu = strcasestr(string,"gugu"); 
    int stringlen = strlen(string);
    
    // if bookmark is found
    if(gugu != NULL)
    {
        // determine the size of values that are not included, aka extrons if intron is present
        int gulen = strlen(gugu);
        glen = stringlen - gulen;
        
        // passes gugu pointer to then search for "AGAG" bookmark
        const char* search = gugu;
        char* agag  = strcasestr(search,"agag");
        if( agag != NULL)
        {
            // we know an intron is present so copy extron to global and set pointer to end 
            memcpy(gpt,string,glen);
            gpt = gpt + glen;
            
            //move agag pointer to after agag and past the rest of the line in rna 
            agag = agag + 4;
            const char* rest = agag;
            rna(rest);
            return;
            
            
        }
        // otherwise no intron located and store the rest of the string in global
        else
        {
            memcpy(gpt,string, stringlen);
            glen+= stringlen;
            return;
        }

    }
    // otherwise no intron located and strore the rest of the string in global
    else
    {
        memcpy(gpt,string, stringlen);
        glen+= stringlen;
        return;
    }
    
    return;
 
}