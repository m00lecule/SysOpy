#include "stat_directory.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <errno.h>

void print_file(const char * name , struct stat * stats){

  char buff[PATH_MAX];
  sprintf(buff,"%s ",name);

  if(S_ISSOCK(stats->st_mode)){
    strcat(buff,"sock ");
  }else if(S_ISDIR(stats->st_mode)){
    strcat(buff,"dir ");
  }else if(S_ISREG(stats->st_mode)){
    strcat(buff,"file ");
  }else if(S_ISLNK(stats->st_mode)){
    strcat(buff,"slink ");
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

int search(const char *path, short mode,time_t ref){
  DIR * dir;
  struct dirent *entry = NULL;
  struct stat stats;

  if((dir = opendir(path)) == NULL)
    return -2;

  while((entry = readdir(dir)) != NULL ){

    char curr_path[512];
    sprintf(curr_path,"%s/%s",path,entry->d_name);

    if(lstat(curr_path,&stats)<0){
      printf("Error: %s\n",strerror(errno));
      return -1;
    }

    if( S_ISDIR(stats.st_mode) ){

      if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
        continue;

      search(curr_path,mode,ref);
    }

    if((mode == 0 && ref == stats.st_mtime) || (mode == 1 && ref < stats.st_mtime) || (mode == 2 && ref > stats.st_mtime) )
      print_file(curr_path,&stats);
    }

    closedir(dir);
    return 0;
}
