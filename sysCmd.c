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

void runSysCommand(char *cmd[], int inBG)
{
    pid_t taskID = fork();

    if (taskID == -1)
    {
        perror("Error in fork");
        exit(1);
    }
    else if (!taskID)
    {
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
        }
        else
        {
            time_t beginTime, finishTime;
            beginTime = time(NULL);
            waitpid(taskID, NULL, 0);
            finishTime = time(NULL);
            printf("command took %ld seconds\n", (long)finishTime - beginTime);
            if(difftime(finishTime, beginTime) > 2) {
                TLE_time = (int) difftime(finishTime, beginTime);
                snprintf(TLE_process, MAX_COMMAND_NAME, "%s", cmd[0]);
            } else {
                TLE_time = 0;
            }
        }
    }
}

void sysCmd_func(char *command, char *save_ptr, int isBackground)
{
    char *token = strtok_r(NULL, " ", &save_ptr);
    // char cmd[MAX_ARG][MAX_ARG_SIZE];
    char **cmd = (char **)malloc(100 * sizeof(char *));
    if (command[strlen(command) - 1] == '\n')
    {
        command[strlen(command) - 1] = '\0';
    }
    cmd[0] = strdup(command);
    // printf("Command is: %s", command);
    int argIdx = 1;
    while (token != NULL)
    {
        if (token[strlen(token) - 1] == '\n')
        {
            token[strlen(token) - 1] = '\0';
        }
        cmd[argIdx] = strdup(token);
        argIdx++;
        token = strtok_r(NULL, " ", &save_ptr);
    }
    cmd[argIdx] = NULL;

    runSysCommand(cmd, isBackground);
}
