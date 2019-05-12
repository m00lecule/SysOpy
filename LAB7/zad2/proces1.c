#define key_trucker "\\michaldygas3"
#define shared_key_int "\\michal"
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
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


sem_t * mutex;


void exit_fun(void){
  sem_close(mutex);
  }


int shared_int;
int * ptr_int;


int main(int argc, char** argv){

  atexit(exit_fun);

  if ((mutex = sem_open (key_trucker, O_RDWR)) == NULL) {
       perror ("sem_open"); exit (1);
        }

        if((shared_int = shm_open(shared_key_int,O_RDWR | O_CREAT,0666)) == -1){
          printf("3\n");
          exit(1);
        }

        if((ptr_int =(int *) mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE, MAP_SHARED,shared_int,0)) == (void *) -1){
          printf("9\n");
          exit(1);
        }


  time_t currtime;
  while(1){
    if(sem_wait(mutex) < 0)
  {
    printf("Negative value when waiting for semaphore!\n");
    exit(1);
}
    time(&currtime);
    sleep(1);
    ptr_int[0]++;
    printf("%d %s",ptr_int[0],ctime(&currtime) );
    if(sem_post(mutex) < 0)
  {
    printf("Negative value when waiting for semaphore!\n");
    exit(1);
}
  usleep(10);
  }

  return 0;
}
