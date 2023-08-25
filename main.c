#include "headers.h"

int main()
{
    initialise_terminal();
    // Keep accepting commands
    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        reviewBGTasks();
        prompt();
        char input[4096];
        fgets(input, 4096, stdin);
        distribute(input);        
    }
}
