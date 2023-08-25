#include "headers.h"

void proclore_func(char *save_ptr)
{
    char *token = strtok_r(NULL, " ", &save_ptr);

    // Initializing variables
    pid_t pid;
    char buffer[4096];
    char buffer1[4096];
    char path[4096];
    FILE *file;

    if (token == NULL)
    {
        pid = getpid(); // get the process ID of the current process
    }
    else
    {
        token[strlen(token) - 1] = '\0';
        pid = atoi(token);
    }

    // Get process info from /proc/[pid]/stat
    sprintf(path, "/proc/%d/stat", pid);
    file = fopen(path, "r");
    fgets(buffer, sizeof(buffer), file);
    fclose(file);

    int process_pid, process_ppid, process_pgrp, process_session, process_tty_nr, process_tpgid;
    char process_comm[4096], process_state;
    long int process_vsize;

    sscanf(buffer,
           "%d %s %c %d %d %d %d %d %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s "
           "%*s %*s %*s %*s %*s %*s %*s %*s %*s %ld",
           &process_pid, process_comm, &process_state, &process_ppid, &process_pgrp, &process_session,
           &process_tty_nr, &process_tpgid, &process_vsize);

    // Get full executable path from /proc/[pid]/exe
    sprintf(path, "/proc/%d/exe", pid);
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    buffer[len] = '\0';

    sprintf(path, "/proc/%d/status", pid);
    file = fopen(path, "r");

    long int virtual_kilo = 0; // Initialize virtual memory size in KB
    while (fgets(buffer1, sizeof(buffer1), file))
    {
        if (sscanf(buffer1, "VmSize: %ld kB", &virtual_kilo) == 1)
        {
            break;
        }
    }
    fclose(file);

    // kilo bytes to bytes
    long int virtual_bytes = virtual_kilo * 1024;


    char *foreground_background = "";
    if (process_tpgid == process_pgrp)
    {
        foreground_background = "+";
    }

    printf("PID : %d\n", process_pid);
    printf("Process Status : %c%s\n", process_state, foreground_background);
    printf("Process Group : %d\n", process_pgrp);
    printf("Virtual Memory : %ld bytes\n", virtual_bytes);
    if (strstr(buffer, home_dir) != NULL)
    {
        // this function checks if home_dir is a substring of executable path
        char *temp = strstr(buffer, home_dir);
        printf("Executable Path : ~%s\n", temp + strlen(home_dir));
    }
    else
    {
        printf("Executable Path: %s\n", buffer);
    }
}