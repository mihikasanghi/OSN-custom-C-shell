#include "headers.h"

void killBGProcess()
{
    struct Node *current = activity;

    while (current != NULL)
    {
        kill(current->pid, 9);
        current = current->next;
    }
}

void ping_func(char **token)
{
    int signalNum = atoi(token[2]) % 32;
    pid_t pid = atoi(token[1]);

    kill(pid, signalNum);

    printf("Sent signal %d to process with pid %d\n", signalNum, pid);
}