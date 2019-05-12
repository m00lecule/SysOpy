#define key_trucker "\\michaldygas"
#define key_loader "\\michaldygas1"
#define shared_key_int "\\int"
#define shared_key_pid "\\pid"
#define shared_key_time "\\time"
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
#include <sys/mman.h>

sem_t * mutex_trucker;
sem_t * mutex_loaders;

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


void init_semaphores_and_shared_int();
void exit_fun(void);
void sigint_handle();
double time_diff(struct timeval x, struct timeval y);



int main(int argc, char** argv){

  signal(SIGINT,sigint_handle);

  if( argc == 4){
    K = atoi(argv[1]);
    M = atoi(argv[2]);
    X = atoi(argv[3]);
  }else{
    printf("ARGUMENTS : [K - max parcel num] [M - max parcels weight] [X - max truck load]\n");
    return 1;
  }

  init_semaphores_and_shared_int();

  int load;
  struct timeval currtime;

  while(1){

    printf("Empty truck arrives\n");
    printf("Waiting to load\n");

    sem_post(mutex_loaders);
    sem_wait(mutex_trucker);
    usleep(10);

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
  }

  return 0;
}

void exit_fun(void){

  if(ptr_int != NULL){
    ptr_int = (int*) shared;
    ptr_int[2] = -1;
  }

  sem_post(mutex_loaders);

  sem_unlink(key_trucker);
  sem_unlink(key_loader);
  sem_close(mutex_loaders);
  sem_close(mutex_trucker);

  shm_unlink(shared_key_int);
  shm_unlink(shared_key_pid);
  shm_unlink(shared_key_time);

  if(shared != NULL)
    munmap(shared,(K+3)*sizeof(int));

  if(ptr_pid != NULL)
    munmap(ptr_pid,K*sizeof(pid_t));

  if(ptr_time != NULL)
    munmap(ptr_time,K*sizeof(struct timeval));
}

void sigint_handle(int sig){
  exit_fun();
  exit(1);
}


void init_semaphores_and_shared_int(){
  if((mutex_trucker = sem_open(key_trucker, O_RDWR | O_CREAT, 0666, 0)) == SEM_FAILED){
    printf("1\n");
    exit_fun();
    exit(1);
  }

  if((mutex_loaders = sem_open(key_loader, O_RDWR | O_CREAT, 0666, 1)) == SEM_FAILED){
    printf("2\n");
    exit_fun();
    exit(1);
  }

  if((shared_int = shm_open(shared_key_int,O_RDWR | O_CREAT,0666)) == -1){
    printf("3\n");
    exit_fun();
    exit(1);
  }

  if((shared_pid = shm_open(shared_key_pid,O_RDWR | O_CREAT,0666)) == -1){
    printf("4\n");
    exit_fun();
    exit(1);
  }

  if((shared_time = shm_open(shared_key_time,O_RDWR | O_CREAT,0666)) == -1){
    printf("5\n");
    exit_fun();
    exit(1);
  }


  if((ftruncate(shared_int,(K)*sizeof(int))) == -1){
    printf("6\n");
    exit_fun();
    exit(1);
  }

  if((ftruncate(shared_pid,(K)*sizeof(pid_t))) == -1){
    printf("7\n");
    exit_fun();
    exit(1);
  }

  if((ftruncate(shared_time,(K)*sizeof(struct timeval))) == -1){
    printf("8\n");
    exit_fun();
    exit(1);
  }

  if((ptr_int =(int *) mmap(NULL,(K)*sizeof(int),PROT_READ | PROT_WRITE, MAP_SHARED ,shared_int,0)) == (void *) -1){
    printf("9\n");
    exit_fun();
    exit(1);
  }

  if((ptr_pid =(pid_t *) mmap(NULL,(K)*sizeof(pid_t),PROT_READ | PROT_WRITE,  MAP_SHARED ,shared_pid,0)) == (void *) -1){
    printf("10\n");
    exit_fun();
    exit(1);
  }

  if((ptr_time =(struct timeval *) mmap(NULL,(K)*sizeof(struct timeval),PROT_READ | PROT_WRITE,  MAP_SHARED ,shared_time,0)) == (void *) -1){
    printf("11\n");
    exit_fun();
    exit(1);
  }


  shared = (void *)ptr_int;
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
