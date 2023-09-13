#ifndef __SYSCMD_H
#define __SYSCMD_H

#include "headers.h"

void reviewBGTasks();
void sysCmd_func(char *command, char **token, int isBackground);
void addToActFile(pid_t taskID, char *cmd);
void removeFromActFile(pid_t taskID);

#endif