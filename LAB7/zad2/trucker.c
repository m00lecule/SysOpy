#define key_trucker "\\michaldygas"
#define key_loader "\\michaldygas1"
#define shared_key_int "\\int"
#define shared_key_pid "\\pid"

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
void * shared = NULL;
int * ptr_int = NULL;
pid_t * ptr_pid = NULL;
struct timeval * ptr_time = NULL;
int * ptr_load;
int K;
int M;
int X;
int shared_size;

void init_semaphores_and_shared_int();
void exit_fun(void);
void unload_shared_memory();
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

  shared_size = (K+3)*sizeof(int) + K*sizeof(pid_t) + K*sizeof(struct timeval);

  init_semaphores_and_shared_int();


  while(1){

    printf("Empty truck arrives\n");
    printf("Waiting to load\n");

    sem_post(mutex_loaders);
    sem_wait(mutex_trucker);

    unload_shared_memory();

    printf("Truck is fully loaded\n");
  }

  return 0;
}

void exit_fun(void){

  if(ptr_int != NULL){
    *(ptr_load) = -1;
  }

  sem_post(mutex_loaders);
  sem_post(mutex_loaders);


  sem_unlink(key_trucker);
  sem_unlink(key_loader);
  sem_close(mutex_loaders);
  sem_close(mutex_trucker);

  sleep(1);
  unload_shared_memory();

  shm_unlink(shared_key_int);

  if(shared != NULL)
    munmap(shared,shared_size);
}

void sigint_handle(int no){
  exit(1);
}

void init_semaphores_and_shared_int(){
  if((mutex_trucker = sem_open(key_trucker, O_RDWR | O_CREAT, 0666, 0)) == SEM_FAILED){
    exit_fun();
    exit(1);
  }

  if((mutex_loaders = sem_open(key_loader, O_RDWR | O_CREAT, 0666, 1)) == SEM_FAILED){
    exit_fun();
    exit(1);
  }

  if((shared_int = shm_open(shared_key_int,O_RDWR | O_CREAT,0666)) == -1){
    exit_fun();
    exit(1);
  }


  if((ftruncate(shared_int,shared_size) == -1)){
    exit_fun();
    exit(1);
  }

  if((ptr_int =(int *) mmap(NULL,shared_size,PROT_READ | PROT_WRITE, MAP_SHARED ,shared_int,0)) == (void *) -1){
    exit_fun();
    exit(1);
  }


  shared = (void *)ptr_int;
  ptr_int[0]=K;
  ptr_int[1]=M;
  ptr_int[2]=0;
  ptr_load = &ptr_int[2];

  ptr_int = &ptr_int[3];
  ptr_pid = (pid_t *)&ptr_int[K];
  ptr_time = (struct timeval *)&ptr_pid[K];

  for(int i = 0 ; i < K ; ++i){
    ptr_int[i]=-1;
  }
}

void unload_shared_memory(){
  int load = 0;
  struct timeval currtime;
  int i;
  for(i = 0 ; i < K && ptr_int[i] != -1 &&  load + ptr_int[i] <= X; ++i){
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

}

double time_diff(struct timeval x , struct timeval y){
    double x_ms , y_ms , diff;

    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;

    diff = (double)y_ms - (double)x_ms;

    return diff;
}
