#define _XOPEN_SOURCE 500
#include "nftw_directory.h"
#include <ftw.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>

static time_t time_h = 0;
static short MODE = 0;

int file_action(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {

  if(ftwbuf->level!=0 &&(((MODE == 0) && (time_h == sb->st_mode)) || ((MODE == 1) && (time_h > sb->st_mode)) || ((MODE == 2 )&& (time_h < sb ->st_mode)))){
    print_file(fpath,tflag,sb);
  }

   return 0;
 }


void print_file(const char * name , short tflag  ,const struct stat * stats){


  char buff[4098];
  sprintf(buff,"%s ",name);

  if( (tflag == FTW_SL) || (tflag == FTW_SLN) ){
    strcat(buff,"slink ");
  }else if(S_ISSOCK(stats->st_mode)){
    strcat(buff,"sock ");
  }else if(S_ISDIR(stats->st_mode)){
    strcat(buff,"dir ");
  }else if(S_ISREG(stats->st_mode)){
    strcat(buff,"file ");
  }else if(S_ISFIFO(stats->st_mode)){
    strcat(buff,"fifo ");
  }else if(S_ISBLK(stats->st_mode)){
    strcat(buff,"blk dev ");
  }else if(S_ISCHR(stats->st_mode)){
    strcat(buff,"char dev ");
  }

  char buff2[128];

  sprintf(buff2,"size: %ld, last opened: %s, last mod: %s",stats->st_size,strtok(ctime(&(stats->st_atime)),"\n"),strtok(ctime(&(stats->st_mtime)),"\n"));
  strcat(buff,buff2);
  printf("%s\n",buff);
}

int search(const char * path,short mode, time_t ref){
  time_h = ref;
  MODE = mode;

  nftw(path,file_action,64,FTW_PHYS);

  return 0;
}
