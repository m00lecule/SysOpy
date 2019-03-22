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

void set_root(const char * root_name){
  if(ROOT == NULL)
    free(ROOT);

  ROOT = calloc(sizeof(char),strlen(root_name));
  strcpy(ROOT,root_name);
}

int search(const char *path){
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



      pid_t childPid = fork();

      if(childPid > 0){
        int value;
        pid_t ret_pid = waitpid(childPid,&value,0);

        if ( WIFEXITED(value) ) {
        int es = WEXITSTATUS(value);
        printf("Exit status was %d\n", es);
    }

        printf("%d %d\n",ret_pid, value );

      }else if(childPid==0){
        printf("%s: PID %d \n",curr_path+strlen(ROOT)+1,getpid());
        execl("/bin/ls", "ls",curr_path, "-l",NULL);
        exit(4);
      }

      search(curr_path);
    }
  }

    closedir(dir);
    return 0;
}
