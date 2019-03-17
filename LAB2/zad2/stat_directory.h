#ifndef STAT_DIRECTORY
#define STAT_DIRECTORY
#define __USE_XOPEN
#define _GNU_SOURCE
#include <sys/types.h>
#include <time.h>

int search(const char*,short,time_t);

#endif
