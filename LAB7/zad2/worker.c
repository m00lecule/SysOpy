#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define shared_key_pid "\\pid"

int shared_int;
int * ptr_int;
pid_t * child_pid = NULL;

int N;
int shared_size;

void spawning_loaders(char**);

int main(int argc, char** argv){

  N = atoi(argv[1]);

  char* av[4];
  av[0] = strdup("./loader");

  if( argc == 3 ){
    av[2] = strdup(argv[2]);
    av[3] = NULL;
    spawning_loaders(av);
  }else{
    av[2] = NULL;
    spawning_loaders(av);
  }

  while (wait(NULL) > 0);

//  munmap(shared_int,shared_size);

  return 0;
}

void spawning_loaders(char * av []){
  pid_t child;
  char buff[4];

  for(int i = 1 ; i <= N; ++i){
    if((child = fork()) == 0){
        sprintf(buff,"%d",i);
        av[1] = strdup(buff);
        execvp(av[0],av);
    }
  }
}
