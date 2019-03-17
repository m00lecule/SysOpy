#ifdef STAT
  #include "stat_directory.h"
#endif

#ifndef STAT
  #include "nftw_directory.h"
#endif

#ifndef _XOPEN_SOURCE
  #define _XOPEN_SOURCE 500
#endif

#include <sys/types.h>
#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char** argv){

  struct tm time;
  time_t time_func;
  char* buff = realpath(argv[1],NULL);

  strptime(argv[2],"%Y %m %d %H:%M" ,&time);
  time_func = mktime(&time);

  short mode=0;

  if(strcmp(argv[3],"<")==0){
    mode=1;
  }else if(strcmp(argv[3],">")==0){
    mode=2;
  }else if(strcmp(argv[3],"=")==0){
    mode=0;
  }else{
    printf(" <path> [Year Month Day Hour:Min] [=,>,<]");
    return -1;
  }

  search(buff,mode,time_func);

  free(buff);

  return 0;
}
