#ifndef __SYSCMD_H
#define __SYSCMD_H

#include "headers.h"

void reviewBGTasks();
void sysCmd_func(char *command, char **token, int isBackground);
struct Node {
    char args[50][100]; // Assuming each node contains a string
    int pid;
    char state[10];
    struct Node* next;
};
void runSysCommand(char *cmd[], int inBG, char** token);
void printList(struct Node* head);
void deleteNode(struct Node **head, int pid);
void insertInOrder(struct Node **head, char **args, char *state, int pid);

#endif