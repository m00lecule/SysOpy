#define _GNU_SOURCE
#define MAX(x, y) ((x) > (y) ? (x) : (y))
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
#define MAXLENGT 1024

int W;
int H;
int C;
int M;
unsigned short * I = NULL;
float * K = NULL;
unsigned short * J = NULL;

typedef struct args{
  int index;
  int index1;
}args;


void get_image(const char* name ){
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen(name, "r");
  if (fp == NULL)
      exit(EXIT_FAILURE);

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
  int j = 0;
  char* ptr = NULL;
  while ((read = getline(&line, &len, fp)) != -1) {
      j = 0;
      while((ptr = strtok_r(line, " " ,&line))){
        sscanf(ptr,"%hu",I + i*W + j);
        j++;
    }
      i++;
  }
}

void get_filter(const char * name){
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen(name, "r");
  if (fp == NULL)
      exit(1);

  if((read = getline(&line, &len, fp)) != -1) {
  }

  if((read = getline(&line, &len, fp)) != -1) {
      sscanf(line,"%d %d",&C,&C);
      K = (float *)malloc(C*C*sizeof(float));
  }


  if((read = getline(&line, &len, fp)) != -1) {
  }

  int i = 0;
  int j = 0;
  char* ptr = NULL;
  while ((read = getline(&line, &len, fp)) != -1) {
      j = 0;
      while((ptr = strtok_r(line, " " ,&line))){
        sscanf(ptr,"%f",K + i*C + j);
        j++;
    }
      i++;
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
        sum+=I[(MAX(0,x - (int)ceil(C/2) + i)) + (MAX(0,y - (int)ceil(C/2) + j))*W] * K[(i) + (j)*C];
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

  FILE * fp = fopen(name,"ab+");

  for(int i = 0 ; i < H ; ++i){
    buff[0]='\0';
    for(int j = 0 ; j < W -1 ; ++j){
        sprintf(buff_digit,"%hu ",J[j + i * W]);
        strcat(buff,buff_digit);
    }
    sprintf(buff_digit,"%hu\n",J[W-1 + i * W]);
    strcat(buff,buff_digit);
    if(fwrite(buff,sizeof(char),strlen(buff),fp));
  }

}


void * thread_fun(void* input){
  printf("tid: %ld %d\n",pthread_self(),*(int*)input);
  int * ret = (int *)malloc(1*sizeof(int));

  *(ret) = *((int*)input);
  *(ret)+=1;
  pthread_exit(ret);
}

int main(int argc, char** argv){

  get_image(argv[1]);
  get_filter("f.txt");

  const int N = 5;
  M=N;
  pthread_t threads[N];
  int args[N];
  void * ret;

  for(int i = 0 ; i < N ; ++i){
    args[i]=i;
    pthread_create(threads + i , NULL, block_fun,(void *)&args[i]);
  }


  for(int i = 0 ; i < N ; ++i){
    pthread_join(threads[i],&ret);
    printf("%f\n",*(double*)ret);

    free(ret);
  }

  print_filtered_image();

  create_file("michal.txt");
  free(I);
  free(K);
  free(J);
  return 0;
}
