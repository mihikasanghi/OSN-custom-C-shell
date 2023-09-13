#ifndef __PEEK_H
#define __PEEK_H

int compare(const void *a, const void *b);
void display_long_format(const char *path, const char *entry_name);
void list_directory(const char *path, int show_all, int long_format);
int peek_func(char** token);

#endif