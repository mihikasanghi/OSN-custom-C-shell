#include "headers.h"

#define MAX_ENTRIES 2048

int compare(const void *str1, const void *str2)
{
    char *const *pp1 = str1;
    char *const *pp2 = str2;
    return strcmp(*pp1, *pp2);
}

void display_long_format(const char *path, const char *entry_name)
{
    struct stat file_stat;
    char filepath[1024];
    struct passwd *pw;
    struct group *grp;

    snprintf(filepath, sizeof(filepath), "%s/%s", path, entry_name);

    if (stat(filepath, &file_stat) == -1)
    {
        perror("stat");
        return;
    }

    printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");

    printf(" %lu ", file_stat.st_nlink);

    if ((pw = getpwuid(file_stat.st_uid)) != NULL)
    {
        printf("%s ", pw->pw_name);
    }
    else
    {
        printf("%d ", file_stat.st_uid);
    }

    if ((grp = getgrgid(file_stat.st_gid)) != NULL)
    {
        printf("%s ", grp->gr_name);
    }
    else
    {
        printf("%d ", file_stat.st_gid);
    }

    printf("%5ld ", file_stat.st_size);

    char mtime[80];
    strftime(mtime, sizeof(mtime), "%b %d %H:%M", localtime(&file_stat.st_mtime));
    // print directories in blue, files in white, executables in green
    if (S_ISDIR(file_stat.st_mode))
    {
        printf("%s \e[0;34m%s\e[0m\n", mtime, entry_name);
    }
    else if (file_stat.st_mode & S_IXUSR)
    {
        printf("%s \e[0;32m%s\e[0m\n", mtime, entry_name);
    }
    else
    {
        printf("%s %s\n", mtime, entry_name);
    }
    // printf("%s %s\n", mtime, entry_name);
}

void list_directory(const char *path, int show_all, int long_format)
{
    DIR *d;
    struct dirent *dir;
    char *entries[MAX_ENTRIES];
    int count = 0;

    d = opendir(path);
    if (d == NULL)
    {
        perror("Unable to read directory");
        return;
    }

    while ((dir = readdir(d)) != NULL)
    {
        if (!show_all && dir->d_name[0] == '.')
        {
            continue;
        }
        entries[count++] = strdup(dir->d_name);
    }
    closedir(d);

    qsort(entries, count, sizeof(*entries), compare);

    for (int i = 0; i < count; i++)
    {
        if (long_format)
        {
            display_long_format(path, entries[i]);
        }
        else
        {
            printf("%s\n", entries[i]);
        }
    }

    free(*entries);

    if (!long_format)
    {
        printf("\n");
    }
}

int peek_func(char **token)
{
    int idx = 1;
    // char *token = strtok_r(NULL, " ", &save_ptr);
    int show_all = 0;
    int long_format = 0;
    char path[4096];
    strcpy(path, cwd);

    while (token[idx] != NULL)
    {
        // printf("flag: %s.", token[idx]);
        if (strcmp(token[idx], "-a") == 0 || strcmp(token[idx], "-a\n") == 0)
        {
            show_all = 1;
        }
        else if (strcmp(token[idx], "-l") == 0 || strcmp(token[idx], "-l\n") == 0)
        {
            long_format = 1;
        }
        else if (strcmp(token[idx], "-al") == 0 || strcmp(token[idx], "-al\n") == 0)
        {
            show_all = 1;
            long_format = 1;
        }
        else if (strcmp(token[idx], "-la") == 0 || strcmp(token[idx], "-la\n") == 0)
        {
            show_all = 1;
            long_format = 1;
        }
        else
        {
            char destination_dir[4096];
            if (token[idx][0] == '~' && token[idx][1] == '/')
            {
                // go to this absolute path
                strcpy(destination_dir, home_dir);
                strcat(destination_dir, "/");
                strcat(destination_dir, token[idx] + 2);
                // remove '' from destination_dir
                destination_dir[strlen(destination_dir) - 1] = '\0';
            }
            else if (token[idx][0] == '~')
            {
                strcpy(destination_dir, home_dir);
            }

            else if (token[idx][0] == '-')
            {
                // go to previous directory
                strcpy(destination_dir, prev_dir);
            }
            else if (token[idx][0] == '.' && token[idx][1] == '.')
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
            else if (token[idx][0] == '.' && token[idx][1] == '/')
            {
                // add path to current path
                // printf("%s\n", cwd);
                strcpy(destination_dir, cwd);
                strcat(destination_dir, "/");
                strcat(destination_dir, token[idx] + 2);
                destination_dir[strlen(destination_dir) - 1] = '\0';
            }
            else if (token[idx][0] == '.')
            {
                // do nothing
                strcpy(destination_dir, cwd);
            }
            else
            {
                // path is absolute, so no modification needed
                strcpy(destination_dir, cwd);
                strcat(destination_dir, "/");
                strcat(destination_dir, token[idx]);
                destination_dir[strlen(destination_dir) - 1] = '\0';
            }
            struct stat st;
            if (stat(destination_dir, &st) == 0 && S_ISDIR(st.st_mode))
            {
                strcpy(path, destination_dir);
            }
            else
            {
                printf("Invalid path\n");
                return 0;
            }
        }
        // token[idx] = strtok_r(NULL, " ", &save_ptr);
        idx++;
    }

    list_directory(path, show_all, long_format);
    return 0;
}