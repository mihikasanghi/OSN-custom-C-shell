#ifndef __PROMPT_H
#define __PROMPT_H

extern char user[4096], cwd[4096], home_dir[4096], prev_dir[4096], system_name[4096];
extern char history[15][4096];
extern int history_full;
extern int history_index;
extern int TLE_time;
extern char TLE_process[512];
void prompt();

#endif