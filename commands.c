#include "headers.h"

void initialise_terminal()
{

    int fd = open(".pastevents.txt", O_RDWR | O_CREAT, 0777);
    // Get number of lines in file
    int lines = 0;
    char c;
    while (read(fd, &c, 1) != 0)
    {
        if (c == '\n')
        {
            lines++;
        }
    }
    // printf("%d\n", lines);
    if (lines >= 15)
    {
        history_full = 1;
        history_index = 14;
    }
    else
    {
        history_index = lines;
    }
    close(fd);
    // printf("%d\n", history_index);

    getcwd(cwd, 4096);
    strcpy(home_dir, cwd);
    strcpy(prev_dir, cwd);
    getlogin_r(user, 4096);
    gethostname(system_name, 4096);
}

void warp_func(char *save_ptr)
{
    char *token = strtok_r(NULL, " ", &save_ptr);
    // printf("%s\n", token);
    if (token == NULL)
    {
        strcpy(prev_dir, cwd);
        strcpy(cwd, home_dir);
        printf("%s\n", home_dir);
    }
    while (token != NULL)
    {
        char destination_dir[4096];
        if (token[0] == '~' && token[1] == '/')
        {
            // go to this absolute path
            strcpy(destination_dir, home_dir);
            strcat(destination_dir, "/");
            strcat(destination_dir, token + 2);
            // remove '' from destination_dir
            destination_dir[strlen(destination_dir) - 1] = '\0';
        }
        else if (token[0] == '~')
        {
            strcpy(destination_dir, home_dir);
        }

        else if (token[0] == '-')
        {
            // go to previous directory
            strcpy(destination_dir, prev_dir);
        }
        else if (token[0] == '.' && token[1] == '.')
        {
            // go to parent directory
            // find the last '/'
            strcpy(destination_dir, cwd);
            int i = strlen(destination_dir) - 1;
            while (destination_dir[i] != '/')
            {
                i--;
            }
            destination_dir[i] = '\0';
        }
        else if (token[0] == '.' && token[1] == '/')
        {
            // add apth to current path
            // printf("%s\n", cwd);
            strcpy(destination_dir, cwd);
            strcat(destination_dir, "/");
            strcat(destination_dir, token + 2);
            destination_dir[strlen(destination_dir) - 1] = '\0';
        }
        else if (token[0] == '.')
        {
            // do nothing
            strcpy(destination_dir, cwd);
        }
        else if (token[0] == '/')
        {
            // go to absolute path
            strcpy(destination_dir, token);
        }
        else
        {
            // path is absolute, so no modification needed
            strcpy(destination_dir, cwd);
            strcat(destination_dir, "/");
            strcat(destination_dir, token);
            destination_dir[strlen(destination_dir) - 1] = '\0';
        }

        struct stat st;
        if (stat(destination_dir, &st) == 0 && S_ISDIR(st.st_mode))
        {
            strcpy(prev_dir, cwd);
            strcpy(cwd, destination_dir);
            printf("%s\n", destination_dir);
        }
        else
        {
            printf("Invalid path\n");
        }

        // printf("%s\n", token);
        token = strtok_r(NULL, " ", &save_ptr);
    }

    return;
}