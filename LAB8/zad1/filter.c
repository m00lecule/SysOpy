#define _GNU_SOURCE
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#define MAXLENGT 10000

int W;
int H;
int C;
int M;
int MAX_VAL = 0;
unsigned short * I = NULL;
float * K = NULL;
unsigned short * J = NULL;
pthread_t * threads = NULL;
int* args = NULL;



void exit_fun(){
  if( I != NULL){
    free(I);
  }

  if( K != NULL){
    free(K);
  }

  if(J != NULL){
    free(J);
  }
  if(threads != NULL){
    free(threads);
  }

  if(args!=NULL){
    free(args);
  }
}

void get_image(const char* name ){
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  if((fp = fopen(name, "r")) == NULL){
    printf("Couldnt open image %s",name );
    exit(1);
  }

  if((read = getline(&line, &len, fp)) != -1) {
  }
  if((read = getline(&line, &len, fp)) != -1) {
  }

  if((read = getline(&line, &len, fp)) != -1) {
      sscanf(line,"%d %d",&W,&H);
      I = (unsigned short *)malloc(W*H*sizeof(unsigned short));
      J = (unsigned short *)malloc(W*H*sizeof(unsigned short));

      if(I == NULL || J == NULL)
        exit(1);
  }


  if((read = getline(&line, &len, fp)) != -1) {
  }

  int i = 0;
  char* ptr = NULL;
  const char* delim = " \n\r";
  while ((read = getline(&line, &len, fp)) != -1) {
      ptr = strtok(line,delim);
      while(ptr != NULL){
        sscanf(ptr,"%hu",&I[i]);
        //printf("%s %d %hu \n",ptr,i,I[i]);
        ptr = strtok(NULL,delim);
        i++;
    }
  }
}

void get_filter(const char * name){
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  if((fp = fopen(name, "r"))==NULL){
    exit(1);
  }

  if((read = getline(&line, &len, fp)) != -1) {
      sscanf(line,"%d",&C);
      K = (float *)malloc(C*C*sizeof(float));
  }

  int i = 0;
  char* ptr = NULL;
  if((read = getline(&line, &len, fp)) != -1) {
      while((ptr = strtok_r(line, " " ,&line))){
        sscanf(ptr,"%f",K + i++);
    }
  }
}

void print_image(){
  for(int j = 0 ; j < H; ++j){
    for(int i = 0 ; i < W ; ++i)
      printf("%hu ",I[i+j*W]);

    printf("\n");
  }
}

void print_filtered_image(){
  for(int j = 0 ; j < H; ++j){
    for(int i = 0 ; i < W ; ++i)
      printf("%hu ",J[i+j*W]);

    printf("\n");
  }
}

void print_filter(){
  for(int j = 0 ; j < C; ++j){
    for(int i = 0 ; i < C ; ++i)
      printf("%f ",K[i+j*C]);

    printf("\n");
  }
}

void splot(int x){
  float sum;
  for( int y = 0 ; y < H ; ++y){
    sum = 0;
    for(int i = 0 ; i < C ; ++i){
      for(int j = 0 ; j < C ; ++j){
        sum+=I[MIN(MAX(0,x - (int)ceil(C/2) + i),W-1) + ( MIN(MAX(0,y - (int)ceil(C/2) + j), H -1 ))*W] * K[(i) + (j)*C];
      }
    }

    sum /= C*C;
    J[x + y*W] = (unsigned int)ceil(sum);
  }
}

double time_diff(struct timeval x , struct timeval y){
    double x_ms , y_ms , diff;

    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;

    diff = (double)y_ms - (double)x_ms;

    return diff;
}

void * block_fun(void* in){
  int x = *(int*)in;

  struct timeval t1,t2;
  double * elapsedTime = (double *)malloc(sizeof(double));

  gettimeofday(&t1,NULL);

  for( int i = x * ceil(W/M) ; i < (x+1)*ceil(W/M) && i < W ; ++i)
    splot(i);

  gettimeofday(&t2,NULL);

  *(elapsedTime) = time_diff(t1,t2);

  pthread_exit(elapsedTime);
}

void * interleaved(void* in){
  int x = *(int *)in;

  struct timeval t1,t2;
  double * elapsedTime = (double *)malloc(sizeof(double));

  gettimeofday(&t1,NULL);

  for(int i = x ; i < W ; i+=M)
    splot(i);

  gettimeofday(&t2,NULL);

  *(elapsedTime) = time_diff(t1,t2);

  pthread_exit(elapsedTime);
}

void create_file(const char * name){
  char buff[MAXLENGT];
  char buff_digit[20];

  FILE * fp = fopen(name,"w+");
  sprintf(buff_digit,"P2\n%d %d\n255\n",W,H);
  fwrite(buff_digit,sizeof(char),strlen(buff_digit),fp);
  for(int i = 0 ; i < H ; ++i){
    buff[0] = '\0';
    for(int j = 0 ; j < W -1 ; ++j){
        sprintf(buff_digit,"%hu ",J[j + i * W]);
        strcat(buff,buff_digit);
    }
    sprintf(buff_digit,"%hu\n",J[W-1 + i * W]);
    strcat(buff,buff_digit);

    if(i != H -1)
      if(fwrite(buff,sizeof(char),strlen(buff),fp));
  }

  if(fwrite(buff,sizeof(char),strlen(buff)-1,fp));

  fclose(fp);
}

int main(int argc, char** argv){

  atexit(exit_fun);

  if(argc != 6){
    printf("ARGS : [THREADS NO] [block/interleaved] [image file name] [filter file name] [filtered file name]\n");
    return 0;
  }

  get_image(argv[3]);
  get_filter(argv[4]);

  int N = atoi(argv[1]);
  M=N;
  threads = (pthread_t *)malloc(N*sizeof(pthread_t));
  args = (int *) malloc(N*sizeof(int));
  void * ret;
  double sum = 0;
  double main_time_difference = 0;

  void * (*fun_ptr)(void *) = NULL;

  if(strcmp(argv[2],"block") ==0 ){
    fun_ptr=&block_fun;
  }else if(strcmp(argv[2],"interleaved") == 0){
    fun_ptr=&interleaved;
  }else{
    exit(1);
  }


  struct timeval t1,t2;
  gettimeofday(&t1,NULL);

  for(int i = 0 ; i < N ; ++i){
    args[i]=i;
    pthread_create(threads + i , NULL, fun_ptr,(void *)&args[i]);
  }

  for(int i = 0 ; i < N ; ++i){
    pthread_join(threads[i],&ret);
    sum += *(double*)ret;
    free(ret);
  }

  // print_filtered_image();

  create_file(argv[5]);
  gettimeofday(&t2,NULL);
  main_time_difference = time_diff(t1,t2);
  printf("W:%d H:%d \n",W,H);
  printf("MAIN : %f\n",main_time_difference );
  printf("THREADS : %f\n",sum);

  return 0;
}
