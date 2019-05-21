#include <string.h>
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
#include <time.h>
char buff[9086];
char buff_temp[10];

int main(int argc,char** argv){
  srand(time(NULL));
  FILE * f = fopen(argv[2],"w+");
  int N = atoi(argv[1]);
  sprintf(buff,"%d\n",N);
  fwrite(buff,sizeof(char),strlen(buff),f);
  buff[0]='\0';

  for(int i = 0 ; i < N*N - 1 ; ++i){
    sprintf(buff_temp,"%f ",(float)rand()/((float)RAND_MAX));
    strcat(buff,buff_temp);
  }

  sprintf(buff_temp,"%f",(float)rand()/((float)RAND_MAX));
  strcat(buff,buff_temp);
  fwrite(buff,sizeof(char),strlen(buff),f);

  fclose(f);
  return 0;
}
