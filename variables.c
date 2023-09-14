char user[4096], cwd[4096], home_dir[4096], prev_dir[4096], system_name[4096];
char history[15][4096];
int history_full = 0;
int history_index = 0;
int TLE_time = 0;
char TLE_process[512];
struct Node {
    char args[50][100];
    int pid;
    char state[10];
    struct Node* next;
};
struct Node* activity;
int fgID = -1;
char** fgName;
