#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lexical_analysis.h"


int main ()
{
    printf("I still can't compile a damn thing! Get to work, Kyle!\n");
    
    printf("\nTesting isKeyword.\n");
    while (true)
    {
        char yourWord[256];
        printf("Please enter a single word: "); scanf("%s", yourWord);
        getchar();
        if (isKwd(yourWord))
        {
            printf("%s is a keyword.\n", yourWord);
        
        } else if (strcmp(yourWord, "q") == 0)
        {
            printf("Right! I am done! Get me compiling code, Kyle!\n");
            return 0;
        
        } else
        {
            printf("%s is not a keyword.\n", yourWord);
        }
    }

    return 0;
}
