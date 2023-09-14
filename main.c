#include "headers.h"

void sigint()
{
    if (fgID != -1)
    {
        kill(fgID, 9);
    }
    return;
}

void sigtstp()
{
    if (fgID != -1)
    {
        kill(fgID, SIGTSTP);
        insertInOrder(&activity, fgName, "Stopped", fgID);
        fgID = -1;
    }
    return;
}

void sigchild() {
    int pid;
    while(pid = waitpid(-1, NULL, WNOHANG) > 0);
    return;
}

int main()
{
    initialise_terminal();
    activity = NULL;
    signal(SIGINT, sigint); // Ctrl + C
    signal(SIGTSTP, sigtstp); // Ctrl + Z
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, sigchild); 
    // Keep accepting commands
    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt();
        char input[4096];
        fgets(input, 4096, stdin);
        // if eof exit
        if (feof(stdin)) // Ctrl + D
        {
            printf("\n");
            killBGProcess();
            exit(0);
        }
        reviewBGTasks();
        distribute(input);
    }
}
