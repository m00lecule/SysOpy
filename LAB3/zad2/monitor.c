#define _GNU_SOURCE
#include "monitor.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <libgen.h>

static enum State state = RAM;
static char* file_buffer=NULL;
static time_t last_mod=0;


void set_mode(enum State st){ state = st;}

int begin_monitoring(const char * path , unsigned int sec_to_finish){
  FILE * file = fopen(path,"r");

  if(!file)
    return -1;

  char buff[512];
  char file_path[256];
  float interval;

  while ( fgets ( buff, sizeof buff, file ) != NULL ){
      char* pch = strtok(buff," ");

      if(pch){
        fclose(file);
        return -2;
      }

      strcpy(file_path,pch);
      pch = strtok (NULL, " ");
      interval=strtod(pch,NULL);

      pid_t childPid = fork();

      if(childPid<0){
        fclose(file);
        return -3;
      }

      if(childPid == 0){
        file_monitoring(file_path,sec_to_finish,interval);
      }
  }

    fclose(file);
    return 0;
};

int file_monitoring(const char * path, unsigned int exec_time, float interval){
  int cycle_counter=0;

  struct timespec start_time, end_time;

  clock_gettime(CLOCK_REALTIME,&start_time);
  clock_gettime(CLOCK_REALTIME,&end_time);

  while((unsigned int)(end_time.tv_sec - start_time.tv_sec) < exec_time){
    copy(path);
    sleep(interval);
    cycle_counter++;
    clock_gettime(CLOCK_REALTIME,&end_time);
  }

  if(!file_buffer)
    free(file_buffer);

  exit(cycle_counter);
}

int copy(const char * path){
  FILE * file = fopen(path,"r");

  if(file == NULL)
    return -1;

  struct stat status;
  lstat(path,&status);

  if(state == RAM){
    if(file_buffer == NULL){
      last_mod=status.st_mtime;

      size_t size = fseek(file,0,SEEK_END);

      file_buffer=calloc(sizeof(char),size);
      fseek(file,0,SEEK_SET);

      if( fread(file_buffer,sizeof(char),size,file) < size)
        return -2;
    }else if(last_mod < status.st_mtime){
      time_t curr_time;
      time(&curr_time);

      char buff[256];
      char buff_time[50];
      char path_copy[512];

      strftime(buff_time, 50, "_%Y-%m-%d_%H-%M-%S", localtime(&curr_time));

      strcpy(path_copy,path);

      sprintf(buff,"archwium/%s%s",basename(path_copy),buff_time);
      FILE * copy_file = fopen(buff,"w");
      fwrite(file_buffer,sizeof(char),strlen(file_buffer),copy_file);
      fclose(copy_file);

      free(file_buffer);
      size_t size = fseek(file,0,SEEK_END);
      fseek(file,0,SEEK_SET);
      file_buffer = calloc(sizeof(char),size);

      fread(file_buffer,sizeof(char),size,file);
      last_mod=status.st_mtime;
    }

  }else{
    if(last_mod<status.st_mtime){
      last_mod=status.st_mtime;

        pid_t childPid = fork();
        time_t curr_time;
        time(&curr_time);

        if(childPid<0){}

        if(childPid==0){
          char buff[256];
          char buff_time[50];
          char path_copy[512];
          strcpy(path_copy,path);

          strftime(buff_time, 50, "_%Y-%m-%d_%H-%M-%S", localtime(&curr_time));

          sprintf(buff,"archwium/%s%s",basename(path_copy),buff_time);
          strcpy(path_copy,path);
          char* const av[]={"cp",path_copy , buff, NULL};
          execvp("cp", av);
          exit(0);
        }else if( childPid>0){
          wait(NULL);
        }
    }

  }
  fclose(file);
  return 0;
}


void wait_for_end(){
  pid_t wpid;
  int status;

  while ((wpid = wait(&status)) > 0){
    if(WIFEXITED(status))
      printf("Proces PID %d utworzył %d kopii pliku",wpid,WEXITSTATUS(status));
  }

}
