#ifndef __SYSCMD_H
#define __SYSCMD_H

#include "headers.h"

// typedef struct {
//     pid_t pid;
//     char commandName[100];
// };
void reviewBGTasks();
void sysCmd_func(char *command, char *save_ptr, int isBackground);

#endif