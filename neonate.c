#include "headers.h"

void die(const char *s)
{
    perror(s);
    exit(1);
}

static int is_pid(const char *name)
{
    while (*name)
    {
        if (!isdigit(*name++))
            return 0;
    }
    return 1;
}

int get_most_recent_pid()
{
    DIR *dir;
    struct dirent *ent;
    struct stat s;
    char path[512];
    int latest_pid = -1;
    time_t latest_time = 0;

    if ((dir = opendir("/proc")) == NULL)
        return latest_pid;

    for (ent = readdir(dir); ent != NULL; ent = readdir(dir))
    {
        if (is_pid(ent->d_name))
        {
            snprintf(path, sizeof(path), "/proc/%s", ent->d_name);
            if (stat(path, &s) == 0 && s.st_mtime > latest_time)
            {
                latest_time = s.st_mtime;
                latest_pid = strtol(ent->d_name, NULL, 10);
            }
        }
    }

    closedir(dir);
    return latest_pid;
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int is_x_pressed()
{
    char c;
    ssize_t bytesRead = read(STDIN_FILENO, &c, 1);
    if (bytesRead > 0 && c == 'x')
    {
        return 1;
    }
    return 0;
}

void neonate_func(char **token)
{

    char time_arg_char[32];
    strcpy(time_arg_char, token[2]);

    if (time_arg_char[strlen(time_arg_char) - 1] == '\n')
    {
        time_arg_char[strlen(time_arg_char) - 1] = '\0';
    }

    int time_arg = atoi(time_arg_char);

    char c;
    while (1)
    {
        setbuf(stdout, NULL);
        enableRawMode();
        while (1)
        {
            pid_t pid = get_most_recent_pid();
            if (pid != -1)
            {
                printf("%d\n", pid);
            }

            int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
            fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

            if (is_x_pressed())
            {
                fcntl(STDIN_FILENO, F_SETFL, flags);
                disableRawMode();
                return;
            }

            sleep(time_arg);

            fcntl(STDIN_FILENO, F_SETFL, flags);
        }
        disableRawMode();
    }
}
