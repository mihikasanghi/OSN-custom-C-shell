#include "headers.h"

typedef struct
{
    int search_files;
    int search_dirs;
    int exec_flag;
    int found;
} SearchOptions;

void remove_extension(char *str)
{
    char *dot = strrchr(str, '.');
    if (dot)
    {
        *dot = '\0'; // Remove the extension
    }
}

void traverse_and_search(const char *dir_name, const char *search, SearchOptions *options)
{
    DIR *dir;
    struct dirent *entry;
    struct stat entry_stat;
    char path[1024];

    if (!(dir = opendir(dir_name)))
    {
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char dirname[1024];
            strcpy(dirname, entry->d_name);
            remove_extension(dirname);

            if (options->search_dirs && strcmp(search, dirname) == 0)
            {
                snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
                printf("\033[34m%s\033[0m\n", path); // Directory in blue
                options->found = 1;

                if (options->exec_flag)
                {
                    if (chdir(path) == 0)
                    {
                        closedir(dir);
                        return;
                    }
                    else
                    {
                        printf("Missing permissions for task!\n");
                        closedir(dir);
                        return;
                    }
                }
            }
            snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
            traverse_and_search(path, search, options);
        }
        else if (entry->d_type == DT_REG)
        {
            char filename[1024];
            strcpy(filename, entry->d_name);
            remove_extension(filename);

            if (options->search_files && strcmp(search, filename) == 0)
            {
                snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
                stat(path, &entry_stat);

                if (options->exec_flag && (entry_stat.st_mode & S_IRUSR))
                {
                    printf("\033[32m%s\033[0m\n", path); // File in green
                    options->found = 1;
                    FILE *fp = fopen(path, "r");
                    char ch;
                    while ((ch = fgetc(fp)) != EOF)
                    {
                        putchar(ch);
                    }
                    fclose(fp);
                    closedir(dir);
                    return;
                }
                else if (!options->exec_flag)
                {
                    printf("\033[32m%s\033[0m\n", path); // File in green
                    options->found = 1;
                }
            }
        }
    }
    closedir(dir);
}

int seek_func(char **token)
{
    // char *token = strtok_r(NULL, " ", &save_ptr);
    int idx = 1;
    SearchOptions options = {1, 1, 0, 0};
    const char *search_term = NULL;
    char *target_directory = "."; // default to current directory

    int flag_dir = 0, flag_d = 0, flag_f = 0, flag_e = 0, flag_con = 0;
    // Argument parsing
    while (token[idx] != NULL)
    {
        if (token[idx][0] == '-')
        {
            if (strcmp(token[idx], "-d") == 0)
            {
                options.search_dirs = 1;
                options.search_files = 0;
                flag_d = 1;
                if(flag_f == 1) {
                    printf("Invalid flags!\n");
                    flag_con = 1;
                }
            }
            else if (strcmp(token[idx], "-f") == 0)
            {
                options.search_files = 1;
                options.search_dirs = 0;
                flag_f = 1;
                if(flag_d == 1) {
                    printf("Invalid flags!\n");
                    flag_con = 1;
                }
            }
            else if (strcmp(token[idx], "-e") == 0)
            {
                options.exec_flag = 1;
            }
        }
        else
        {
            if(token[idx][strlen(token[idx])-1] == '\n') {
                token[idx][strlen(token[idx])-1] = '\0';
                if(flag_dir == 1) {
                    target_directory = token[idx];
                }
            }
            else {
                flag_dir = 1;
            }
            search_term = token[idx];
            break;
        }
        // token[idx] = strtok_r(NULL, " ", &save_ptr);
        idx++;
    }

    if(flag_con) {
        return 0;
    }

    if (search_term == NULL)
    {
        printf("Missing search term!\n");
        return 0;
    }

    traverse_and_search(target_directory, search_term, &options);

    if (!options.found)
    {
        printf("No match found!\n");
    }

    return 0;
}