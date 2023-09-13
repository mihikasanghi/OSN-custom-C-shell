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
const char *actFile = "/tmp/activities_output";

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
                removeFromActFile(allTasks[idx].taskPid);
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

void addToActFile(pid_t taskID, char *cmd)
{
    int fileDesc0 = open(actFile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    close(fileDesc0);
    FILE *actFilePtr = fopen(actFile, "r");
    if (!actFilePtr)
    {
        perror("Error opening activities file");
        return;
    }

    char actCmd[128];
    char taskNum[32];
    sprintf(taskNum, "%d", taskID);
    strcpy(actCmd, taskNum);
    strcat(actCmd, ":");
    strcat(actCmd, cmd);
    strcat(actCmd, "-Running\n");

    char word[128];
    char words[2048][128];
    int numWords = 0;

    while (fscanf(actFilePtr, "%s", word) != EOF)
    {
        strcpy(words[numWords], word);
        numWords++;
    }

    fclose(actFilePtr);

    int i;
    for (i = numWords - 1; i >= 0 && strcmp(actCmd, words[i]) < 0; i--)
    {
        strcpy(words[i + 1], words[i]);
    }
    strcpy(words[i + 1], actCmd);
    numWords++;

    FILE *actFilePtr2 = fopen(actFile, "w");
    if (!actFilePtr2)
    {
        perror("Error opening activities file");
        return;
    }

    // Write the words back to the file
    for (i = 0; i < numWords; i++)
    {
        fprintf(actFilePtr2, "%s\n", words[i]);
    }

    // Close the file
    fclose(actFilePtr2);
}

void removeFromActFile(pid_t taskID)
{
    int fileDesc1 = open(actFile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    close(fileDesc1);
    FILE *actFilePtr = fopen(actFile, "r");
    if (!actFilePtr)
    {
        perror("Error opening activities file");
        return;
    }

    char taskNum[32];
    sprintf(taskNum, "%d", taskID);

    char word[128];
    char words[2048][128];
    int numWords = 0;

    while (fscanf(actFilePtr, "%s", word) != EOF)
    {
        if (strncmp(word, taskNum, strlen(taskNum)) == 0)
        {
            continue;
        }
        strcpy(words[numWords], word);
        numWords++;
    }

    fclose(actFilePtr);

    FILE *actFilePtr2 = fopen(actFile, "w");
    if (!actFilePtr2)
    {
        perror("Error opening activities file");
        return;
    }

    // Write the words back to the file
    for (int i = 0; i < numWords; i++)
    {
        fprintf(actFilePtr2, "%s\n", words[i]);
    }

    // Close the file
    fclose(actFilePtr2);
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

            addToActFile(taskID, cmd[0]);
        }
        else
        {
            time_t beginTime, finishTime;
            beginTime = time(NULL);
            waitpid(taskID, NULL, 0);
            finishTime = time(NULL);
            // printf("command took %ld seconds\n", (long)finishTime - beginTime);
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

    runSysCommand(cmd, isBackground);
}
