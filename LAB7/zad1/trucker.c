#define key 51
#define shared_key 15
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
#include <sys/time.h>

int mutex;
int shared_int;
int shared_pid;
int shared_time;
void * shared = NULL;
int * ptr_int = NULL;
pid_t * ptr_pid = NULL;
struct timeval * ptr_time = NULL;
int * ptr_load;
int K;
int M;
int X;

// 0 - TRUCK MUTEX 1 -WORKERS MUTEX
union semun {
  int val;
  struct semid_ds *buf;
  ushort *array;
};

void init_semaphores_and_shared_int();
void exit_fun(void);
void sigint_handle();
double time_diff(struct timeval x, struct timeval y);



int main(int argc, char** argv){

  signal(SIGINT,sigint_handle);
  atexit(exit_fun);

  if( argc == 4){
    K = atoi(argv[1]);
    M = atoi(argv[2]);
    X = atoi(argv[3]);
  }else{
    printf("ARGUMENTS : [K - max parcel num] [M - max parcels weight] [X - max truck load]\n");
    return 1;
  }

  init_semaphores_and_shared_int();
  union semun arg;
  struct sembuf sem_action;
  sem_action.sem_flg = SEM_UNDO;
  int load;
  struct timeval currtime;

  int iter = 5;

  while(iter--){

    printf("Empty truck arrives\n");
    printf("Waiting to load\n");

    arg.val = 1;

    if (semctl(mutex, 1, SETVAL, arg) == -1) {
       exit(1);
    }

    sem_action.sem_num = 0;
    sem_action.sem_op = -1;
    semop(mutex,&sem_action,1);

    load = 0;
    int i;
    for(i = 0 ; i < K && load + ptr_int[i] <= X; ++i){
      gettimeofday(&currtime,NULL);
      load +=ptr_int[i];
      printf("TRUCK %d: SPACE: %d W: %d, PID: %d , T: %f\n",getpid(), X - load ,ptr_int[i],ptr_pid[i],time_diff(ptr_time[i],currtime));
    }
    *ptr_load -= load;

    for(int j = i ; j < K ; ++j){
      ptr_int[j-i] = ptr_int[j];
      ptr_pid[j-i] = ptr_pid[j];
      ptr_time[j-i] = ptr_time[j];
    }


    for(int j = K - i ; j < K ; ++j ){
      ptr_int[j]=-1;
    }

    for(int j = 0 ; j < K ; ++j ){
      printf("%d ",ptr_int[j]);
    }

    printf("Truck is fully loaded\n");

    arg.val = 0;
    if (semctl(mutex, 0, SETVAL, arg) == -1) {
       exit(1);
    }
  }

  return 0;
}

void exit_fun(void){

  if(ptr_int != NULL){
    ptr_int = (int*) shared;
    ptr_int[2] = -1;
  }
  semctl(mutex,0,IPC_RMID,0);
  shmctl(shared_int,IPC_RMID,NULL);
  shmctl(shared_pid,IPC_RMID,NULL);
  shmctl(shared_time,IPC_RMID,NULL);
}

void sigint_handle(int sig){
  exit(1);
}


void init_semaphores_and_shared_int(){
  if((mutex = semget(key,2,0666 | IPC_CREAT)) == -1){
    exit(1);
  }

  union semun arg;
  arg.val = 0;

  if (semctl(mutex, 0, SETVAL, arg) == -1) {
     exit(1);
  }

  arg.val = 1;
  if (semctl(mutex, 1, SETVAL, arg) == -1) {
     exit(1);
  }

  if((shared_pid = shmget(shared_key + 1, K * sizeof(pid_t),0666 | IPC_CREAT)) == -1){
    exit(1);
  }

  if((ptr_pid = (pid_t*) shmat(shared_pid,NULL,0)) == (void *)-1){
    exit(1);
  }

  if((shared_time = shmget(shared_key+2,K * sizeof(struct timeval),0666 | IPC_CREAT)) == -1){
    exit(1);
  }

  if((ptr_time = (struct timeval*) shmat(shared_time,NULL,0)) == (void *)-1){
    exit(1);
  }

  if((shared_int = shmget(shared_key,(K + 3)*sizeof(int),0666 | IPC_CREAT)) == -1){
    exit(1);
  }

  if((shared = shmat(shared_int,NULL,0)) == (void *)-1){
    exit(1);
  }

  ptr_int = (int*)shared;
  ptr_int[0]=K;
  ptr_int[1]=M;
  ptr_int[2]=0;
  ptr_load = &ptr_int[2];

  ptr_int = &ptr_int[3];

  for(int i = 0 ; i < K ; ++i){
    ptr_int[i]=-1;
  }
}

double time_diff(struct timeval x , struct timeval y){
    double x_ms , y_ms , diff;

    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;

    diff = (double)y_ms - (double)x_ms;

    return diff;
}
