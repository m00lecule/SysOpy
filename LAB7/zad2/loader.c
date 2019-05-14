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
#include <sys/mman.h>
#include <sys/shm.h>
#define key_trucker "\\michaldygas"
#define key_loader "\\michaldygas1"
#define shared_key_int "\\int"

#define SLEEP_TIME 1

sem_t * mutex_trucker;
sem_t * mutex_loaders;

int shared_int;

void * shared = NULL;
int * ptr_int = NULL;
pid_t * ptr_pid = NULL;
struct timeval * ptr_time = NULL;

int K;
int M;
int * load;
int C = -1;
int worker_load;
int shared_size;

union semun {
  int val;
  struct semid_ds *buf;
  ushort *array;
};

void init_semaphores_and_shared_int();
void exit_fun(void);
void loader_action();

int main(int argc, char** argv){

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

  sem_post(mutex_loaders);

  if(ptr_int!=NULL)
    munmap(shared,shared_size);

  sem_close(mutex_loaders);
  sem_close(mutex_trucker);

}

void loader_action(){
  struct timeval currtime;
  gettimeofday(&currtime,NULL);

  sem_wait(mutex_loaders);

  if(*(load) == -1){
    exit(1);
  }

  if(*(load) + worker_load < M ){
    *(load) += worker_load;
    printf("LOADER %d: W: %d load: %d \n",getpid(),worker_load,*(load));
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
          sem_post(mutex_trucker);
        }else{
          sem_post(mutex_loaders);
          usleep(100);
        }
        break;
      }
    }

    //HERES POTENTIAL DEADLOCK IF ANY DATARACE HAPPENS
  }else{
    //CANT PUT PARCEL ON LOADING TAPE - LETS TAKE A BREAK FROM WORK
    printf("cant put %d %d %d\n",getpid(),*(load),worker_load );
    sem_post(mutex_loaders);
    sleep(SLEEP_TIME);
  }
}


void init_semaphores_and_shared_int(){

  if((mutex_trucker = sem_open(key_trucker, O_RDWR )) == SEM_FAILED){
    perror("Couldnt open truckers mutex");
    exit(1);
  }

  if((mutex_loaders = sem_open(key_loader, O_RDWR  )) == SEM_FAILED){
    perror("Couldnt open loaders mutex");
    exit(1);
  }

  if((shared_int = shm_open(shared_key_int, O_RDWR ,0666)) == -1){
    perror("Couldnt open shared memory");
    exit(1);
  }


  if((ptr_int =(int *) mmap(NULL,(2)*sizeof(int),PROT_READ | PROT_WRITE, MAP_SHARED,shared_int,0)) == (void *) -1){
    perror("Couldnt map shared memory");
    exit(1);
  }

  K = ptr_int[0];
  M = ptr_int[1];

  munmap(ptr_int,2*sizeof(int));

  shared_size = (K+3)*sizeof(int) + K*sizeof(pid_t) + K*sizeof(struct timeval);

  if((ptr_int =(int *) mmap(NULL,shared_size,PROT_READ | PROT_WRITE, MAP_SHARED,shared_int,0)) == (void *) -1){
    perror("Couldnt reopen shared memory");
    exit(1);
  }


  shared = (void* )ptr_int;
  load = &ptr_int[2];


  ptr_int = &ptr_int[3];
  ptr_pid = (pid_t *)&ptr_int[K];
  ptr_time = (struct timeval *)&ptr_pid[K];
}
