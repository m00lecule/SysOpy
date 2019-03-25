#ifndef STAT_DIRECTORY
#define STAT_DIRECTORY
#define __USE_XOPEN
#define _GNU_SOURCE
#include <sys/types.h>
#include <time.h>

void set_root(const char*);
int search(const char*);
void free_lib();

#endif
