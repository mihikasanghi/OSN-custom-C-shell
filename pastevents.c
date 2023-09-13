#include "headers.h"

void pastevents_func(char **token)
{
    // char *token = strtok_r(NULL, " ", &save_ptr);
    int idx = 1;
    if (token[idx] == NULL)
    {
        // printf("Hello\n");
        FILE *file = fopen(".pastevents.txt", "r");
        char buffer[4096]; // Adjust the buffer size as needed

        while (fgets(buffer, sizeof(buffer), file) != NULL)
        {
            printf("%s", buffer);
        }
        fclose(file);
    }
    else if (strcmp(token[idx], "purge\n") == 0 || strcmp(token[idx], "purge") == 0)
    {
        FILE *file = fopen(".pastevents.txt", "w");
        fclose(file);
        history_full = 0;
        history_index = 0;
    }
    else if (strcmp(token[idx], "execute") == 0)
    {
        idx++;
        int index = atoi(token[idx]);

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