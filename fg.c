#include "headers.h"

void fg_func(char **token)
{
    char pid[32];
    strcpy(pid, token[1]);
    if (pid[strlen(pid) - 1] == '\n')
    {
        pid[strlen(pid) - 1] = '\0';
    }
    int pidNum = atoi(pid);

    kill(pidNum, SIGCONT);
    waitpid(pidNum, NULL, WUNTRACED);
}