#include "headers.h"

int input_end_pos;
int firstToken;
char *copy_input;

void open_input_file(char *inputFile)
{
    int input_fd = open(inputFile, O_RDONLY);
    if (input_fd < 0)
    {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }
    dup2(input_fd, STDIN_FILENO);
    close(input_fd);
}

void open_output_file(char *outputFile, int append)
{
    int output_fd = append ? open(outputFile, O_WRONLY | O_CREAT | O_APPEND, 0666) : open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (output_fd < 0)
    {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }
    dup2(output_fd, STDOUT_FILENO);
    close(output_fd);
}

void handle_input_redirection(char **args, int *i, char **inputFile)
{
    *inputFile = args[*i + 1];
    args[*i] = NULL;
}

void handle_output_redirection(char **args, int *i, char **outputFile, int *append, int isAppend)
{
    *outputFile = args[*i + 1];
    *append = isAppend;
    args[*i] = NULL;
}

void handle_redirections(char **args, int num_args, char **inputFile, char **outputFile, int *append)
{
    for (int i = 0; i < num_args; i++)
    {
        if (strcmp(args[i], "<") == 0)
            handle_input_redirection(args, &i, inputFile);
        else if (strcmp(args[i], ">") == 0)
            handle_output_redirection(args, &i, outputFile, append, 0);
        else if (strcmp(args[i], ">>") == 0)
            handle_output_redirection(args, &i, outputFile, append, 1);
    }
}

void apply_redirections(char *inputFile, char *outputFile, int append)
{
    if (inputFile)
        open_input_file(inputFile);
    if (outputFile)
        open_output_file(outputFile, append);
}

void execute_command_without_pipe(char *token, int isBackground)
{
    char *token_copy = strdup(token);
    // printf("%s\n", token);
    strcat(token_copy, " ");
    char *save_ptr;
    char *command = strtok_r(token_copy, " \t", &save_ptr);
    char **args = (char **)malloc(sizeof(char *) * 100);
    int idx = 0;
    char *inputFile = NULL;
    char *outputFile = NULL;
    int append = 0;

    while (command != NULL)
    {
        // printf("command: %s\n", command);
        args[idx] = strdup(command);
        command = strtok_r(NULL, " \t", &save_ptr);
        idx++;
    }
    handle_redirections(args, idx, &inputFile, &outputFile, &append);

    int old_stdout = dup(STDOUT_FILENO);
    int old_stdin = dup(STDIN_FILENO);

    apply_redirections(inputFile, outputFile, append);
    args[idx] = '\0';
    if (strcmp(args[0], "warp") == 0 || strcmp(args[0], "warp\n") == 0)
    {
        warp_func(args);
    }
    else if (strcmp(args[0], "peek") == 0 || strcmp(args[0], "peek\n") == 0)
    {
        peek_func(args);
    }
    else if (strcmp(args[0], "pastevents") == 0 || strcmp(args[0], "pastevents\n") == 0)
    {
        pastevents_func(args);
    }
    else if (strcmp(args[0], "proclore") == 0 || strcmp(args[0], "proclore\n") == 0)
    {
        proclore_func(args);
    }
    else if (strcmp(args[0], "seek") == 0 || strcmp(args[0], "seek\n") == 0)
    {
        seek_func(args);
    }
    else if (strcmp(args[0], "activities") == 0 || strcmp(args[0], "activities\n") == 0)
    {
        activities_func(args);
    }
    else if (strcmp(args[0], "iMan") == 0 || strcmp(args[0], "iMan\n") == 0)
    {
        iman_func(args);
    }
    else
    {
        input_end_pos += firstToken ? strlen(token) : strlen(token) + 1;
        firstToken = 0;
        int isBackground = 0;

        if (copy_input[input_end_pos] == '&')
        {
            isBackground = 1;
        }
        if (input_end_pos < strlen(copy_input))
        {
            sysCmd_func(args[0], args, isBackground);
        }
    }
    dup2(old_stdout, STDOUT_FILENO);
    dup2(old_stdin, STDIN_FILENO);
}

void handle_single_command_execution(char **commands, int i, int isLastCommand, int *old_fd, int *fd)
{
    dup2(*old_fd, STDIN_FILENO);
    if (!isLastCommand)
        dup2(fd[1], STDOUT_FILENO);
    close(fd[0]);
    // Assuming 'execute_command_without_pipe' is already defined
    execute_command_without_pipe(commands[i], 0);
    exit(0);
}

void execute_commands_pipeline(char **commands, int num_commands)
{
    int fd[2];
    int old_fd = STDIN_FILENO;

    for (int i = 0; i < num_commands; i++)
    {
        pipe(fd);
        if (fork() == 0)
        {
            handle_single_command_execution(commands, i, i == num_commands - 1, &old_fd, fd);
        }
        else
        {
            close(fd[1]);
            old_fd = fd[0];
        }
    }
    for (int i = 0; i < num_commands; i++)
        wait(NULL);
}

void execute_command_with_pipes(char **commands, int num_commands)
{
    for (int i = 0; i < num_commands; i++)
    {
        if (!commands[i] || strlen(commands[i]) == 0)
        {
            printf("Invalid use of pipe\n");
            return;
        }
    }
    execute_commands_pipeline(commands, num_commands);
}

void distribute(char *input)
{
    if (strcmp(input, "\n") == 0)
    {
        return;
    }

    if (input[strlen(input) - 1] != ';' && input[strlen(input) - 1] != '&')
    {
        input[strlen(input) - 1] = ';';
    }
    char *token = strdup(input);
    copy_input = strdup(input);
    firstToken = 1;
    // printf("Token : %s", token);
    if (strstr(token, "pastevents") == 0 && strcmp(token, "\n") != 0)
    {
        FILE *file = fopen(".pastevents.txt", "r");
        char lastLine[256]; // Adjust the buffer size as needed
        char currentLine[256];
        lastLine[0] = '\0'; // Initialize the last line buffer

        while (fgets(currentLine, sizeof(currentLine), file) != NULL)
        {
            strcpy(lastLine, currentLine);
        }

        fclose(file);

        if (strcmp(lastLine, token) != 0)
        {
            // printf("Equal\n");

            if (history_full)
            {
                // Remove first line of file with file descrpitor fd
                char buffer[4096];
                // read content of file in buffer
                int fd = open(".pastevents.txt", O_RDWR | O_CREAT, 0777);
                lseek(fd, 0, SEEK_CUR);
                read(fd, buffer, 4096);
                close(fd);

                int idx = 0;
                for (int i = 0; i <= 4096; i++)
                {
                    if (buffer[i] == '\n')
                    {
                        idx = i;
                        break;
                    }
                }
                char buffer2[4096];
                strcpy(buffer2, buffer + idx + 1);
                fd = open(".pastevents.txt", O_RDWR | O_CREAT, 0777);
                strcat(buffer2, token);
                lseek(fd, 0, SEEK_CUR);
                write(fd, buffer2, strlen(buffer2));
                close(fd);
            }
            else
            {
                int fd = open(".pastevents.txt", O_RDWR | O_CREAT, 0777);
                lseek(fd, 0, SEEK_END);
                write(fd, token, strlen(token) - 1);
                lseek(fd, 0, SEEK_END);
                write(fd, "\n", strlen("\n"));
                if (history_index == 14)
                {
                    history_full = 1;
                }
                history_index = (1 + history_index) % 15;
            }
        }
    }
    char *save_ptr1;
    input_end_pos = 0;
    token = strtok_r(input, ";&", &save_ptr1);

    while (token != NULL)
    {
        int isBackground = 0;
        const int MAX_COMMANDS = 100;
        char **commands = (char **)malloc(sizeof(char *) * MAX_COMMANDS);
        int num_commands = 0;

        char *token2 = strtok(token, "|");
        while (token2)
        {
            commands[num_commands++] = token2;
            token2 = strtok(NULL, "|");
        }

        if (num_commands > 1)
            execute_command_with_pipes(commands, num_commands);
        else
            execute_command_without_pipe(commands[0], isBackground);

        free(commands);

        // execute_command_without_pipe(token, isBackground);

        // char *token_copy = strdup(token);
        // // printf("%s\n", token);
        // strcat(token_copy, " ");
        // char *save_ptr;
        // char *command = strtok_r(token_copy, " \t", &save_ptr);
        // char** args = (char **)malloc(sizeof(char *) * 100);
        // int idx = 0;
        // while(command != NULL){
        //     // printf("command: %s\n", command);
        //     args[idx] = strdup(command);
        //     command = strtok_r(NULL, " \t", &save_ptr);
        //     idx++;
        // }
        // args[idx] = '\0';
        // if (strcmp(args[0], "warp") == 0 || strcmp(args[0], "warp\n") == 0)
        // {
        //     warp_func(args);
        // }
        // else if (strcmp(args[0], "peek") == 0 || strcmp(args[0], "peek\n") == 0)
        // {
        //     peek_func(args);
        // }
        // else if (strcmp(args[0], "pastevents") == 0 || strcmp(args[0], "pastevents\n") == 0)
        // {
        //     pastevents_func(args);
        // }
        // else if (strcmp(args[0], "proclore") == 0 || strcmp(args[0], "proclore\n") == 0)
        // {
        //     proclore_func(args);
        // }
        // else if (strcmp(args[0], "seek") == 0 || strcmp(args[0], "seek\n") == 0)
        // {
        //     seek_func(args);
        // }
        // else
        // {
        //     input_end_pos +=  firstToken ? strlen(token) : strlen(token) + 1;
        //     firstToken = 0;

        //     if(copy_input[input_end_pos] == '&')
        //     {
        //         isBackground = 1;
        //     }
        //     if(input_end_pos < strlen(copy_input))
        //     {
        //         sysCmd_func(args[0], args, isBackground);
        //     }
        // }
        token = strtok_r(NULL, ";&", &save_ptr1);
    }
}