#ifndef NFTW
#define NFTW
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <sys/types.h>
#include <time.h>

int search(const char*,short,time_t);
void print_file(const char *, short,const struct stat *);
int file_action(const char *, const struct stat *,int, struct FTW *);

#endif
