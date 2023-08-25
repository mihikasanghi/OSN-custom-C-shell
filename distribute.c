#include "headers.h"

void distribute(char *input)
{
    if (strcmp(input, "\n") == 0)
    {
        return;
    }
   
    if(input[strlen(input) - 1] != ';' && input[strlen(input) - 1] != '&')
    {
        input[strlen(input) - 1] = ';';
    }
    char *token = strdup(input);
    char *copy_input = strdup(input);
    int firstToken = 1;
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
                write(fd, token, strlen(token) -1);
                lseek(fd, 0, SEEK_END);
                write(fd, "\n", strlen("\n"));
                if (history_index == 14)
                {
                    printf("history full\n");
                    history_full = 1;
                }
                history_index = (1 + history_index) % 15;
            }
        }
    }
    char *save_ptr1;
    int input_end_pos = 0;
    token = strtok_r(input, ";&", &save_ptr1);
    // printf("Token: %s\n", token);
    // char *token2 = strtok_r(NULL, ";", &save_ptr1);
    // printf("Token2: %s\n", token2);
    // exit(1);

    while (token != NULL)
    {
        char *token_copy = strdup(token);
        // printf("%s\n", token);
        strcat(token_copy, " ");
        char *save_ptr;
        char *command = strtok_r(token_copy, " \t", &save_ptr);
        if (strcmp(command, "warp") == 0 || strcmp(command, "warp\n") == 0)
        {
            warp_func(save_ptr);
        }
        else if (strcmp(command, "peek") == 0 || strcmp(command, "peek\n") == 0)
        {
            peek_func(save_ptr);
        }
        else if (strcmp(command, "pastevents") == 0 || strcmp(command, "pastevents\n") == 0)
        {
            pastevents_func(save_ptr);
        }
        else if (strcmp(command, "proclore") == 0 || strcmp(command, "proclore\n") == 0)
        {
            proclore_func(save_ptr);
        }
        else if (strcmp(command, "seek") == 0 || strcmp(command, "seek\n") == 0)
        {
            seek_func(save_ptr);
        }
        else
        {
            input_end_pos +=  firstToken ? strlen(token) : strlen(token) + 1;
            firstToken = 0;
            int isBackground = 0;
            
            // printf("Input end pos: %d\n", input_end_pos);
            if(copy_input[input_end_pos] == '&')
            {
                isBackground = 1;
            }
            if(input_end_pos < strlen(copy_input)) 
            {
                // printf("delimiter: %c\n", copy_input[input_end_pos]);
                sysCmd_func(command, save_ptr, isBackground);
            }
            // command[strlen(command) - 1] = '\0';
            // printf("ERROR : '%s' is not a valid command\n", command);
            // break;
        }
        // printf("token :%s\n", token);
        token = strtok_r(NULL, ";&", &save_ptr1);
    }
}