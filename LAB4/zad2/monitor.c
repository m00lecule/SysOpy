#include "monitor.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <libgen.h>
#include <fcntl.h>
#include <signal.h>

static char* file_buffer=NULL;
static time_t last_mod=0;
static struct Node * stack = NULL;
static int counter = 0;

typedef enum { false, true} bool;

static bool run = true;

struct Node* initNode(pid_t pid_numb, char* path, u_int8_t interval){
  struct Node* ptr = calloc(1, sizeof(struct Node));
  ptr->next = NULL;
  ptr->interval=interval;
  ptr->pid=pid_numb;
  strcpy(ptr->path,path);
  return ptr;
}
void stop_pid(pid_t pid){
  kill(pid,SIGUSR1);
}

void stop_all(){
  struct Node * ptr  = stack;

  while(ptr!=NULL){
    kill(ptr->pid,SIGUSR1);
    ptr = ptr->next;
  }
}

void start_all(){
  struct Node * ptr  = stack;

  while(ptr!=NULL){
    kill(ptr->pid,SIGUSR2);
    ptr=ptr->next;
  }
}

void start_pid(pid_t pid){
  kill(pid,SIGUSR2);
}

void signal_sigusr1(){
  run = false;
}

void signal_sigusr2(){
  run = true;
}

void signal_sigchld(){
  if(file_buffer)
    free(file_buffer);

  exit(counter);
}



void end(){
  struct Node * ptr = stack;

  while(ptr!=NULL){
      kill(ptr->pid,SIGCHLD);
      ptr=ptr->next;
  }
  wait_for_end();
  freeStack();
  if(file_buffer)
    free(file_buffer);

  exit(0);
}

void freeStack(){

  struct Node * ptr;

  while(stack != NULL){
    ptr = stack->next;
    free(stack);
    stack = ptr;
  }
}

void addToStack( struct Node* ptr){
  ptr->next = stack;
  stack = ptr;
}

void printStack(){
  struct Node * ptr = stack;

  while(ptr!=NULL){
    printf("pid: %d path: %s interval: %d \n",ptr->pid,ptr->path,ptr->interval );
    ptr=ptr->next;
  }
}


int begin_monitoring(const char * path){
  FILE * file = fopen(path,"r");

  if(!file){
    printf("Problems while handling file\n");
    return -1;
  }

  signal(SIGINT,end);

  char buff[512];
  char file_path[256];
  char temp[100];
  int interval;

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

      pch = strtok(NULL, " ");

      interval=atoi(pch);

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

      if(childPid > 0 ){
          struct Node * n = initNode(childPid,file_path,interval);
          addToStack(n);
      }

      if(childPid == 0){
        if(file)
          fclose(file);

        if(file_buffer!= NULL){
          free(file_buffer);
          file_buffer=NULL;
        }

        //freeStack();

        signal(SIGUSR1,signal_sigusr1);
        signal(SIGUSR2,signal_sigusr2);
        signal(SIGCHLD,signal_sigchld);


        if(file_monitoring(file_path,interval)<0){
          printf("Error while running proces for line %d\n",line_counter );
        }
      }

      line_counter++;
  }

  if(file)
    fclose(file);

  return 0;
};

int prepare_static_variables(const char * path){
  int desc = open(path, O_RDONLY);

  if(desc < 0)
    return -1;


  struct stat status;
  lstat(path,&status);
  last_mod=status.st_mtime;

  if(file_buffer == NULL){

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

int file_monitoring(const char * path, int interval){

  struct stat status;

  if(prepare_static_variables(path) < 0){
    exit(-1);
  }

  while(1){
    if(run){
      lstat(path,&status);
        if(last_mod < status.st_mtime){
          copy(path);
          last_mod = status.st_mtime;
          counter++;
        }
        sleep(interval);
      }
    }

  if(file_buffer!=NULL){
    free(file_buffer);
    file_buffer=NULL;
  }
  return 0;
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

  close(desc);
  return 0;
}
