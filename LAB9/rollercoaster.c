#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>


volatile unsigned int cur_trolley = 0;
unsigned int trolley_no;
unsigned int passenger_no;
volatile unsigned int capacity;
volatile unsigned int cur_capacity;
volatile unsigned int running_workers =1;
unsigned int who_should_press_start;
struct timeval start;
int n = 0;

pthread_mutex_t * trolley_mutex = NULL;
pthread_mutex_t load_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t empty_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t load_to_trolley = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t start_mutex = PTHREAD_MUTEX_INITIALIZER;

int * trolley_arg = NULL;
int * passenger_arg = NULL;
pthread_t *trolley_thread;
pthread_t *passenger_thread;

pthread_cond_t empty_cond;
pthread_cond_t* load_cond = NULL;
pthread_cond_t start_cond;

double time_diff(struct timeval x , struct timeval y){
    double x_ms , y_ms , diff;

    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;

    diff = (double)y_ms - (double)x_ms;

    return diff;
}

// void passenger_exit(void* arg){
//   double time_period;
//   struct timeval passenger_time;
//   gettimeofday(&passenger_time,NULL);
//
//
//   time_period = time_diff(start,passenger_time);
//   printf("P %d ENDS %d\n",*(int*)arg, (int)time_period );
// }

void exit_fun(){
  if(trolley_mutex != NULL){
    for(int i = 0 ; i < trolley_no ; ++i)
      pthread_mutex_destroy(&trolley_mutex[i]);

    free(trolley_mutex);
  }

  pthread_mutex_destroy(&load_mutex);
  pthread_mutex_destroy(&empty_mutex);


  if(trolley_arg!=NULL){
    free(trolley_arg);
  }

  if(passenger_arg!=NULL){
    free(passenger_arg);
  }

  if(trolley_thread!=NULL){
    free(trolley_thread);
  }

  if(passenger_thread!=NULL){
    free(passenger_thread);
  }

  if(load_cond != NULL){
    free(load_cond);
  }
}


void init_dynamic_memory(){
  if((trolley_mutex = (pthread_mutex_t*)malloc(trolley_no*sizeof(pthread_mutex_t))) == NULL){
    exit(1);
  }

    for( int i = 0 ; i < trolley_no ; ++i){
      pthread_mutex_init(&trolley_mutex[i],NULL);
    }

    if((trolley_thread = (pthread_t *)malloc(trolley_no*sizeof(pthread_t))) == NULL){
      exit(1);
    }

    if((passenger_thread = (pthread_t*)malloc(passenger_no*sizeof(pthread_t))) == NULL){
      exit(1);
    }

    if((trolley_arg = (int *)malloc(trolley_no*sizeof(int))) == NULL){
      exit(1);
    }

    if((passenger_arg = (int *)malloc(passenger_no*sizeof(int))) == NULL){
      exit(1);
    }

    if((load_cond = (pthread_cond_t *)malloc(trolley_no * sizeof(pthread_cond_t))) == NULL){
      exit(1);
    }

  gettimeofday(&start,NULL);
}


void * trolley_fun(void* arg){
  int no = *(int*)arg;
  struct timeval trolley_time;
  double time_period;

  for(int i = 0 ; i < n ; ++i){
    pthread_mutex_lock(&load_mutex);

    if( no != cur_trolley){
      pthread_cond_wait(&load_cond[no],&load_mutex);
    }
    /***/

    gettimeofday(&trolley_time,NULL);
    time_period = time_diff(start,trolley_time);

    printf("TROLLEY %d ARRIVES T:%d\n",no,(int)time_period );
    fflush(stdout);

    //unload the trolley
    if( i != 0){
      cur_capacity = capacity;
      cur_trolley=no;

      pthread_mutex_unlock(&trolley_mutex[no]);
      //the last one should
      pthread_cond_wait(&empty_cond,&empty_mutex);
    }

    //picking one who should press start
    who_should_press_start = (unsigned int) (rand() % capacity);


    gettimeofday(&trolley_time,NULL);
    time_period = time_diff(start,trolley_time);

    printf("TROLLEY %d STARTS LOADING PASSENGERS, %d SHOULD PRESS START T:%d\n",no,who_should_press_start,(int)time_period );
    fflush(stdout);


    //load the trolley last passenger will own it
    pthread_mutex_lock(&trolley_mutex[no]);
    pthread_mutex_unlock(&load_to_trolley);

    //;ast passener should give a notice
    pthread_cond_wait(&empty_cond,&empty_mutex);
    //closing the doors

    ++cur_trolley;
    cur_trolley%=trolley_no;


    cur_capacity=0;

    gettimeofday(&trolley_time,NULL);
    time_period = time_diff(start,trolley_time);

    printf("TROLLEY %d DEPARTURE T:%d\n",no,(int)time_period );
    fflush(stdout);

    pthread_cond_signal(&load_cond[cur_trolley]);
    pthread_mutex_unlock(&load_mutex);

    //weeeeeee
    usleep(10000);
  }
  //unload
  pthread_mutex_lock(&load_mutex);

  if( no != cur_trolley){
    pthread_cond_wait(&load_cond[no],&load_mutex);
  }
  /***/


  gettimeofday(&trolley_time,NULL);
  time_period = time_diff(start,trolley_time);

  printf("TROLLEY %d ARRIVES T:%d\n",no,(int)time_period );
  fflush(stdout);
  //unload the trolley

  cur_capacity = capacity;
  cur_trolley=no;

  pthread_mutex_unlock(&trolley_mutex[no]);
  //the last one should
  pthread_cond_wait(&empty_cond,&empty_mutex);

  ++cur_trolley;
  cur_trolley%=trolley_no;


  gettimeofday(&trolley_time,NULL);
  time_period = time_diff(start,trolley_time);

  cur_capacity=capacity;
  printf("TROLLEY %d ENDS HIS SHIFT T:%d\n",no,(int)time_period );
  fflush(stdout);

    running_workers = 0;


  pthread_cond_signal(&load_cond[cur_trolley]);
  pthread_mutex_unlock(&load_mutex);


  //end of shift

  return NULL;
}

void * passenger_fun(void* arg){
  int no = *(int*)arg;

  double time_period;
  struct timeval passenger_time;

  while(running_workers){
    //wait for room
    pthread_mutex_lock(&load_to_trolley);
    //pack
    if(!running_workers){
      break;
    }

    gettimeofday(&passenger_time,NULL);


    time_period = time_diff(start,passenger_time);



    printf("PASSENGER %d LOADING T:%d\n",no,(int) time_period );
    fflush(stdout);

    ++cur_capacity;


    if(who_should_press_start == cur_capacity -1 && who_should_press_start != capacity -1){

      pthread_mutex_unlock(&load_to_trolley);
      pthread_cond_wait(&start_cond,&start_mutex);

      gettimeofday(&passenger_time,NULL);
      time_period = time_diff(start,passenger_time);

      printf("PRESSES START %d T: %d\n",no, (int)time_period );
      fflush(stdout);
      pthread_cond_signal(&empty_cond);
      pthread_mutex_unlock(&empty_mutex);

    }else{
      if(cur_capacity == capacity){
        if(who_should_press_start != capacity -1){
          pthread_cond_signal(&start_cond);
          pthread_mutex_unlock(&start_mutex);
        }else{

          gettimeofday(&passenger_time,NULL);
          time_period = time_diff(start,passenger_time);

          printf("PRESSES START %d T: %d\n",no,(int)time_period );
          fflush(stdout);
          pthread_cond_signal(&empty_cond);
          pthread_mutex_unlock(&empty_mutex);
        }
      }else{
        pthread_mutex_unlock(&load_to_trolley);
      }
    }

    pthread_mutex_lock(&trolley_mutex[cur_trolley]);
    //unpack

    gettimeofday(&passenger_time,NULL);
    time_period = time_diff(start,passenger_time);

    printf("UNPACKING %d %d T: %d\n",no,cur_capacity,(int)time_period);
    fflush(stdout);
    if(--cur_capacity == 0){
      pthread_cond_signal(&empty_cond);
      pthread_mutex_unlock(&empty_mutex);
    }
    pthread_mutex_unlock(&trolley_mutex[cur_trolley]);
    usleep(1000);
  }
  gettimeofday(&passenger_time,NULL);
  time_period = time_diff(start,passenger_time);
  pthread_mutex_unlock(&load_to_trolley);


  printf("W %d ENDS SHIFT %d\n", no,(int)time_period );

  return NULL;
}

void init_trolley(){
  for( int i = 0 ; i < trolley_no ; ++i){
    trolley_arg[i] = i;
    pthread_create(&trolley_thread[i],NULL,trolley_fun,(void*)&trolley_arg[i]);
  }
}

void init_passengers(){

  for( int i = 0 ; i < passenger_no ; ++i){
    passenger_arg[i] = i;
    pthread_create(&passenger_thread[i],NULL,passenger_fun,(void*)&passenger_arg[i]);
  }
}

void wait_for_trolley(){
  for(int i = 0 ; i < trolley_no ; ++i){
    pthread_join(trolley_thread[i],NULL);
  }
}

void wait_for_workers(){
  for(int i = 0 ; i < passenger_no ; ++i){
    pthread_join(passenger_thread[i],NULL);
  }
}

int main(int argc, char** argv){
  srand(time(NULL));

  if(argc !=  5){
    printf("ARGS: [TROLLEY NO] [PASSENGER NO] [CAPACITY] [LOOPS]\n");
    return 1;
  }

  trolley_no = atoi(argv[1]);
  passenger_no = atoi(argv[2]);
  capacity = atoi(argv[3]);
  n = atoi(argv[4]);


  init_dynamic_memory();
  init_trolley();
  init_passengers();

  wait_for_trolley();
  wait_for_workers();
  return 0;
}
