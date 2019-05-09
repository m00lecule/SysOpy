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
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define key 51
#define shared_key 15
#define SLEEP_TIME 1
int mutex;

int shared_int;
int shared_pid;
int shared_time;

int * ptr_int;
pid_t * ptr_pid;
struct timeval * ptr_time;

int K;
int M;
int * load;
int C = -1;
int worker_load;

union semun {
  int val;
  struct semid_ds *buf;
  ushort *array;
};

void init_semaphores_and_shared_int();
void exit_fun(void);
void loader_action();

int main(int argc, char** argv){
  srand(time(NULL));
  atexit(exit_fun);

  if( argc == 3){
    C = atoi(argv[2]);
  }
  worker_load = atoi(argv[1]);

  init_semaphores_and_shared_int();

  if(C == -1){
    while(1){
      loader_action();
    }
  }else{
    while(C--){
      loader_action();
    }
  }

  return 0;
}

void exit_fun(void){
  shmdt((void*)ptr_int);
  shmdt((void*)ptr_time);
  shmdt((void*)ptr_pid);
}

void loader_action(){

  if(*(load) == -1){
    exit(1);
  }

  fflush(stdout);
  struct sembuf sem_action;
  sem_action.sem_flg = SEM_UNDO;
  struct timeval currtime;
  gettimeofday(&currtime,NULL);

  sem_action.sem_num = 1;
  sem_action.sem_op = -1;
  semop(mutex,&sem_action,1);

  if(*(load) + worker_load < M ){
    *(load) += worker_load;
    printf("LOADER %d: W: %d \n",getpid(),worker_load);
    fflush(stdout);
    for( int i = 0 ; i < K ; ++i){
      if(ptr_int[i] == -1){
        ptr_int[i] = worker_load;
        ptr_pid[i] = getpid();
        ptr_time[i] = currtime;

        if( i == K - 1 || *(load) == M){

          for(int j = 0 ; j < K ; ++j){
            printf("%d ", ptr_int[j] );
          }

          printf("WAITING FOR TRUCK TO LOAD\n");
          fflush(stdout);
          sem_action.sem_num = 0;
          sem_action.sem_op = 1;
          semop(mutex,&sem_action,1);
        }else{
          sem_action.sem_num = 1;
          sem_action.sem_op = 1;
          semop(mutex,&sem_action,1);
        }

        break;
      }
    }
    //HERES POTENTIAL DEADLOCK IF ANY DATARACE HAPPENS
  }else{
    //CANT PUT PARCEL ON LOADING TAPE - LETS TAKE A BREAK FROM WORK
    sem_action.sem_num = 1;
    sem_action.sem_op = 1;
    semop(mutex,&sem_action,1);
    sleep(SLEEP_TIME);
  }
}


void init_semaphores_and_shared_int(){
  if((mutex = semget(key,2,0666)) == -1){
    exit(1);
  }

  if((shared_pid = shmget(shared_key + 1,0,0666)) == -1){
    exit(1);
  }

  if((ptr_pid = (pid_t*)shmat(shared_pid,NULL,0)) == (void *)-1){
    exit(1);
  }

  if((shared_time = shmget(shared_key+2,0,0666)) == -1){
    exit(1);
  }

  if((ptr_time = (struct timeval*) shmat(shared_time,NULL,0)) == (void *)-1){
    exit(1);
  }

  if((shared_int = shmget(shared_key,0,0666)) == -1){
    exit(1);
  }

  if((ptr_int = (int *) shmat(shared_int,NULL,0)) == (void *)-1){
    exit(1);
  }

  K = ptr_int[0];
  M = ptr_int[1];
  load = &ptr_int[2];
  ptr_int = &ptr_int[3];
}
