#include "headers.h"

void prompt()
{
    // Do not hardcode the prmopt

    if (TLE_time > 2)
    {
        if (strcmp(home_dir, cwd) == 0)
        {
            printf("<\e[0;32m%s\e[0m@\e[0;35m%s\e[0m:~ \e[0;31m%s : %ds\e[0m > ", user, system_name, TLE_process, TLE_time);
        }
        else if (strstr(cwd, home_dir) != NULL)
        {
            // this function checks if home_dir is a substring of cwd
            char *temp = strstr(cwd, home_dir);
            printf("<\e[0;32m%s\e[0m@\e[0;35m%s\e[0m:~ \e[0;33m%s\e[0m\e[0;31m%s : %ds\e[0m > ", user, system_name, temp + strlen(home_dir), TLE_process, TLE_time);
        }
        else if (strcmp(home_dir, cwd) != 0)
        {
            printf("<\e[0;32m%s\e[0m@\e[0;35m%s\e[0m:\e[0;33m%s\e[0m \e[0;31m%s : %ds\e[0m > ", user, system_name, cwd, TLE_process, TLE_time);
        }
        TLE_time = 0;
    }
    else
    {
        if (strcmp(home_dir, cwd) == 0)
        {
            printf("<\e[0;32m%s\e[0m@\e[0;35m%s\e[0m:~> ", user, system_name);
        }
        else if (strstr(cwd, home_dir) != NULL)
        {
            // this function checks if home_dir is a substring of cwd
            char *temp = strstr(cwd, home_dir);
            printf("<\e[0;32m%s\e[0m@\e[0;35m%s\e[0m:~\e[0;33m%s\e[0m> ", user, system_name, temp + strlen(home_dir));
        }
        else if (strcmp(home_dir, cwd) != 0)
        {
            printf("<\e[0;32m%s\e[0m@\e[0;35m%s\e[0m:\e[0;33m%s\e[0m> ", user, system_name, cwd);
        }
    }

}
