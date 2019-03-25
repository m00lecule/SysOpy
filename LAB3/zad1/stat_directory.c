#include "stat_directory.h"
#include <ftw.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#include <dirent.h>

char * ROOT = NULL;

void free_lib(){
  if(ROOT != NULL)
    free(ROOT);
}

void set_root(const char * root_name){
  free_lib();

  ROOT = strdup(root_name);
}

int search(const char *path){
  DIR * dir;
  struct dirent *entry = NULL;
  struct stat stats;

  if((dir = opendir(path)) == NULL){
    printf("Specified directory doesnt exist\n" );
    exit(-2);
  }

  while((entry = readdir(dir)) != NULL ){

    char curr_path[512];
    sprintf(curr_path,"%s/%s",path,entry->d_name);

    if(lstat(curr_path,&stats)<0){
      printf("lstat error: %s\n",strerror(errno));
      exit(-1);
    }


    if( S_ISDIR(stats.st_mode) ){

      if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
        continue;

      pid_t childPid = fork();

      if(childPid > 0){
        waitpid(childPid,NULL,0);
      }else if(childPid==0){
        closedir(dir);
        printf("%s: PID %d \n",curr_path+strlen(ROOT)+1,getpid());
        execl("/bin/ls", "ls",curr_path, "-l",NULL);
        exit(0);
      }

      search(curr_path);
    }
  }

    closedir(dir);
    return 0;
}
