#include "headers.h"

// Update the state of the process
void updateState(struct Node *head, char *newState, int pid)
{
    struct Node *current = head;
    while (current != NULL)
    {
        if (pid == current->pid)
        {
            
            if (newState[0] == 'T')
            {
                strcpy(current->state, "Stopped");
            }
            else if (newState[0] == 'R' || newState[0] == 'S')
            {
                strcpy(current->state, "Running");
            }
            else
            {
                strcpy(current->state, newState);
            }
            return;
        }
        current = current->next;
    }
}

//main function for activities
void activities_func(char **token)
{
    struct Node *current = activity;

    // Traversing all the processes in the linked list to update their process state
    while (current != NULL)
    {
        int pid = current->pid;
        char pDet[100], pLine[150], *tok, newState[2];
        
        sprintf(pDet, "/proc/%d/stat", pid);
        FILE *statusf = fopen(pDet, "r");
        if (!statusf)
        {
            deleteNode(&activity, pid);
            current = current->next;
            continue;
        }
        fgets(pLine, 150, statusf);
        

        tok = strtok(pLine, " ");
        for (int i = 1; i <= 2; i++)
        {
            tok = strtok(NULL, " ");
        }
        

        newState[0] = tok[0];
        newState[1] = '\0';
        

        fclose(statusf);
        

        updateState(activity, newState, pid);
        

        current = current->next;
    }

    // printing all activities
    printList(activity);
    return;
}