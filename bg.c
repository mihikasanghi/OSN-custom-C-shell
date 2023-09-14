#include "headers.h"

void bg_func(char** token){
    char pid[32];
    //converting pid to integer
    strcpy(pid, token[1]);
    if (pid[strlen(pid) - 1] == '\n')
    {
        pid[strlen(pid) - 1] = '\0';
    }
    int pidNum = atoi(pid);

    // send SIGCONT to pidNum
    kill(pidNum, SIGCONT);
}