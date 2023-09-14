#include "headers.h"

#define MAX_COMMAND_NAME 1024
#define BG_MAX 100

struct BGTask
{
    pid_t taskPid;
    char taskCommand[MAX_COMMAND_NAME];
};

struct BGTask allTasks[BG_MAX];
int currentTaskIndex = 0;
const char *tempFile = "/tmp/bg_tasks_output";

struct Node *createNode(char **args, char *state, int pid)
{
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (newNode == NULL)
    {
        perror("Memory allocation failed");
        exit(1);
    }
    // strcpy(newNode->args, args);
    int i = 0;
    while (args[i] != NULL)
    {
        strcpy(newNode->args[i], args[i]);
        i++;
    }
    strcpy(newNode->state, state);
    newNode->pid = pid;
    newNode->next = NULL;
    return newNode;
}

void insertInOrder(struct Node **head, char **args, char *state, int pid)
{
    struct Node *newNode = createNode(args, state, pid);
    int areSame = 1;

    // Special case: Insert at the beginning
    if (*head == NULL || pid < (*head) -> pid)
    {
        newNode->next = *head;
        *head = newNode;
        return;
    }

    struct Node *current = *head;

    // Find the node after which the new node should be inserted
    while (current->next != NULL && current->pid < current->next->pid)
    {
        current = current->next;
    }

    // Insert the new node after the current node
    newNode->next = current->next;
    current->next = newNode;
}

void deleteNode(struct Node **head, int pid)
{
    if (*head == NULL)
    {
        return; // List is empty, nothing to delete
    }

    // Special case: Node to be deleted is the head
    if ((*head)->pid == pid)
    {
        struct Node *temp = *head;
        *head = (*head)->next;
        free(temp);
        return;
    }

    struct Node *current = *head;

    // Find the node before the one to be deleted
    while (current->next != NULL && current->next->pid != pid)
    {
        current = current->next;
    }

    if (current->next != NULL)
    {
        struct Node *temp = current->next;
        current->next = current->next->next;
        free(temp);
    }
}

struct Node *searchNode(struct Node *head, int pid)
{
    struct Node *current = head;
    while (current != NULL)
    {
        if (current->pid == pid)
        {
            return current;
        }
        current = current->next;
    }
    return NULL; // Node with data not found
}

void printList(struct Node *head)
{
    struct Node *current = head;
    while (current != NULL)
    {
        printf("%d : %s - %s\n", current->pid, current->args[0], current->state);
        current = current->next;
    }
}

void showOutputAndClean()
{
    char readBuffer[MAX_COMMAND_NAME];
    FILE *taskFile = fopen(tempFile, "r");
    if (taskFile)
    {
        while (fgets(readBuffer, MAX_COMMAND_NAME, taskFile))
        {
            printf("%s", readBuffer);
        }
        fclose(taskFile);
    }
    remove(tempFile);
}

void reviewBGTasks()
{
    int anyDone = 0;

    for (int idx = 0; idx < currentTaskIndex; idx++)
    {
        int taskStatus;
        pid_t checkResult = waitpid(allTasks[idx].taskPid, &taskStatus, WNOHANG);
        if (checkResult)
        {
            showOutputAndClean();

            if (WIFEXITED(taskStatus))
            {
                printf("%s exited normally (%d)\n", allTasks[idx].taskCommand, allTasks[idx].taskPid);
                deleteNode(&activity, allTasks[idx].taskPid);
            }
            else if (WIFSIGNALED(taskStatus))
            {
                printf("Task %s ended by signal (%d)\n", allTasks[idx].taskCommand, allTasks[idx].taskPid);
            }

            for (int j = idx; j < currentTaskIndex - 1; j++)
            {
                allTasks[j] = allTasks[j + 1];
            }
            currentTaskIndex--;
            idx--;
        }
    }
}

void runSysCommand(char *cmd[], int inBG, char **token)
{
    pid_t taskID = fork();

    if (taskID == -1)
    {
        perror("Error in fork");
        exit(1);
    }
    else if (!taskID)
    {
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, SIG_IGN);
        signal(SIGINT, SIG_DFL);
        if (inBG)
        {
            int fileDesc = open(tempFile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
            dup2(fileDesc, STDOUT_FILENO);
            dup2(fileDesc, STDERR_FILENO);
            close(fileDesc);
        }

        if (execvp(cmd[0], cmd) == -1)
        {
            // perror("Error in exec");
            printf("ERROR: %s is not a valid command\n", cmd[0]);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        if (inBG)
        {
            printf("%d\n", taskID);
            allTasks[currentTaskIndex].taskPid = taskID;
            snprintf(allTasks[currentTaskIndex].taskCommand, MAX_COMMAND_NAME, "%s", cmd[0]);
            currentTaskIndex++;

            insertInOrder(&activity, token, "running", taskID);
        }
        else
        {
            time_t beginTime, finishTime;
            beginTime = time(NULL);
            fgID = taskID;
            int i = 0;
            fgName = (char **)malloc(sizeof(char *) * 100);
            while(token[i] != NULL){
                fgName[i] = strdup(token[i]);
                i++;
            }
            waitpid(taskID, NULL, WUNTRACED);
            fgID = -1;
            finishTime = time(NULL);
            if (difftime(finishTime, beginTime) > 2)
            {
                TLE_time = (int)difftime(finishTime, beginTime);
                snprintf(TLE_process, MAX_COMMAND_NAME, "%s", cmd[0]);
            }
            else
            {
                TLE_time = 0;
            }
        }
    }
}

void sysCmd_func(char *command, char **token, int isBackground)
{
    // char *token = strtok_r(NULL, " ", &save_ptr);
    int idx = 1;
    // char cmd[MAX_ARG][MAX_ARG_SIZE];
    char **cmd = (char **)malloc(100 * sizeof(char *));
    if (command[strlen(command) - 1] == '\n')
    {
        command[strlen(command) - 1] = '\0';
    }
    cmd[0] = strdup(command);
    // printf("Command is: %s", command);
    int argIdx = 1;
    while (token[idx] != NULL)
    {
        if (token[idx][strlen(token[idx]) - 1] == '\n')
        {
            token[idx][strlen(token[idx]) - 1] = '\0';
        }
        cmd[argIdx] = strdup(token[idx]);
        argIdx++;
        // token[idx] = strtok_r(NULL, " ", &save_ptr);
        idx++;
    }
    cmd[argIdx] = NULL;

    runSysCommand(cmd, isBackground, token);
}
