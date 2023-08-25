#include "headers.h"

void pastevents_func(char *save_ptr)
{
    char *token = strtok_r(NULL, " ", &save_ptr);
    if (token == NULL)
    {
        FILE *file = fopen(".pastevents.txt", "r");
        char buffer[4096]; // Adjust the buffer size as needed

        while (fgets(buffer, sizeof(buffer), file) != NULL)
        {
            printf("%s", buffer);
        }
        fclose(file);
    }
    else if (strcmp(token, "purge\n") == 0 || strcmp(token, "purge") == 0)
    {
        FILE *file = fopen(".pastevents.txt", "w");
        fclose(file);
        history_full = 0;
        history_index = 0;
    }
    else if (strcmp(token, "execute") == 0)
    {
        token = strtok_r(NULL, " ", &save_ptr);
        int index = atoi(token);
        //write code to print nth line of a file

        FILE *file = fopen(".pastevents.txt", "r");
        char buffer[512]; // Adjust the buffer size as needed
        int i = 0;
        index = history_index - index;
        while (fgets(buffer, sizeof(buffer), file) != NULL)
        {
            if (i == index)
            {
                distribute(buffer);
                break;
            }
            i++;
        }
    }

    return;
}