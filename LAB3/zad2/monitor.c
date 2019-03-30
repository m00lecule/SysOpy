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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static enum State state = RAM;
static char* file_buffer=NULL;
static time_t last_mod=0;


void set_mode(enum State st){ state = st;}

void begin_monitoring(const char * path , unsigned int sec_to_finish){
  FILE * file = fopen(path,"r");

  if(!file){
    printf("Problems while handling file\n");
    return;
  }

  char buff[512];
  char file_path[256];
  char temp[100];
  float interval;

  int line_counter=0;
  while ( fgets ( buff, sizeof buff, file ) != NULL ){
      char* pch = strtok(buff," ");

      if(pch==NULL){
        printf("Wrong line format %d\n" ,line_counter);
        line_counter++;
        continue;
      }
      strcpy(temp,pch);
      pch = strtok(NULL, " ");

      strcpy(file_path,pch);
      strcat(file_path,temp);

      if(pch==NULL){
        printf("Wrong line format at %d\n",line_counter);
        line_counter++;
        continue;
      }

      interval=strtod(pch,NULL);
      if(interval < 0){
        printf("Wrong interval value at line %d", line_counter );
        line_counter++;
        continue;
      }

      pid_t childPid = fork();

      if(childPid<0){
        printf("Couldnt create process for line %d \n",line_counter );
        line_counter++;
        continue;
      }

      if(childPid == 0){
        if(file)
          fclose(file);

        if(file_buffer!= NULL){
          free(file_buffer);
          file_buffer=NULL;
        }

        if(file_monitoring(file_path,sec_to_finish,interval)<0){
          printf("Error while running proces for line %d\n",line_counter );
        }
      }

      line_counter++;
  }

  if(file)
    fclose(file);
};

int prepare_static_variables(const char * path){
  int desc = open(path, O_RDONLY);

  if(desc < 0)
    return -1;


  struct stat status;
  lstat(path,&status);
  last_mod=status.st_mtime;

  if((state == RAM) && (file_buffer == NULL)){

    size_t size = lseek(desc,0,SEEK_END);

    file_buffer=calloc(sizeof(char),size+1);
    if(file_buffer == NULL){
      close(desc);
      printf("Couldnt allocate memory buffer\n");
      return -2;
    }
    lseek(desc,0,SEEK_SET);

    if( read(desc,file_buffer,size) < size){
      close(desc);
      printf("Error while loading file to memory\n");
      return -2;
      }
    }

    close(desc);
    return 0;
}

int file_monitoring(const char * path, unsigned int exec_time, float interval){
  int cycle_counter=0;

  struct timespec start_time, end_time;

  clock_gettime(CLOCK_REALTIME,&start_time);
  clock_gettime(CLOCK_REALTIME,&end_time);

  struct stat status;

  if(prepare_static_variables(path) < 0){
    exit(-1);
  }

  while((unsigned int)(end_time.tv_sec - start_time.tv_sec) < exec_time){
    lstat(path,&status);

    if(last_mod < status.st_mtime){
      copy(path);
      last_mod = status.st_mtime;
      cycle_counter++;
    }


      sleep(interval);
      clock_gettime(CLOCK_REALTIME,&end_time);
    }

  if(file_buffer!=NULL){
    free(file_buffer);
    file_buffer=NULL;
  }

  exit(cycle_counter);
}

void wait_for_end(){
  pid_t wpid;
  int status;

  while ((wpid = wait(&status)) > 0){
    if(WIFEXITED(status))
      printf("Proces PID %d utworzył %d kopii pliku\n",wpid,WEXITSTATUS(status));
  }

  freeStack();

  if(file_buffer != NULL){
    free(file_buffer);
    file_buffer=NULL;
  }
}

int copy(const char * path){
  int desc = open(path,O_RDONLY);

  if(desc<0){
    printf("PID:%d Couldnt open file: %s\n",getpid(),path);
    return -1;
  }

  if(state == RAM){
    time_t curr_time;
    time(&curr_time);

    char buff[256];
    char buff_time[50];
    char* path_copy;

    strftime(buff_time, 50, "_%Y-%m-%d_%H-%M-%S", localtime(&curr_time));

    path_copy = strdup(path);

    sprintf(buff,"archiwum/%s%s",basename(path_copy),buff_time);

    int desc_copy = open(buff, O_WRONLY|O_CREAT , 0777);

    if(desc_copy<0){
      printf("PID: %d couldnt open file: %s\n",getpid(),buff);
      return -1;
    }

    if(write(desc_copy, file_buffer, strlen(file_buffer)) < strlen(file_buffer)){
          printf("PID: %d error while writing to file: %s\n",getpid(),buff);
          close(desc_copy);
          return -2;
    }

    close(desc_copy);

    if(file_buffer!=NULL)
      free(file_buffer);


    size_t size = lseek(desc,0,SEEK_END);
    lseek(desc,0,SEEK_SET);
    file_buffer = calloc(sizeof(*file_buffer),size+1);

    if(file_buffer == NULL){
      close(desc);
      printf("PID: %d couldnt allocate memory for file content\n",getpid() );
      return -2;
    }

    if(read(desc,file_buffer,size)<size){
      close(desc);
      printf("PID: %d error while loading file to memory \n",getpid() );
      return -3;
    }

    free(path_copy);
  }else{
    pid_t childPid = fork();
    time_t curr_time;
    time(&curr_time);

    if(childPid<0){}

    if(childPid==0){
      close(desc);
      if(file_buffer!=NULL){
        free(file_buffer);
        file_buffer=NULL;
      }
      char buff[256];
      char buff_time[50];
      char path_copy[512];
      strcpy(path_copy,path);

      strftime(buff_time, 50, "_%Y-%m-%d_%H-%M-%S", localtime(&curr_time));

      sprintf(buff,"archiwum/%s%s",basename(path_copy),buff_time);
      strcpy(path_copy,path);
      char* const av[]={"cp",path_copy , buff, NULL};
      execvp("cp", av);
      exit(0);
      }else if( childPid>0){
        wait(NULL);
      }
    }
  close(desc);
  return 0;
}
