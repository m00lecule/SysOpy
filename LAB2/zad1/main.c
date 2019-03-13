#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "filehandler.h"

#define BILLION 1000000000

void print_time(char* header ,struct timespec* starttime, struct timespec* endtime, const struct tms * start, const struct tms* end){
    double sys_time = (double) (end->tms_stime - start->tms_stime) / sysconf(_SC_CLK_TCK);
    sys_time+=(double) (end->tms_cstime - start->tms_cstime) / sysconf(_SC_CLK_TCK);

    double user_time = (double) (end->tms_utime - start->tms_utime) / sysconf(_SC_CLK_TCK);
    user_time += (double) (end->tms_cutime - start->tms_cutime) / sysconf(_SC_CLK_TCK);

    double real_time = (double) (endtime->tv_sec - starttime->tv_sec);
    real_time += (double) (endtime->tv_nsec - starttime->tv_nsec) / BILLION;

    char buff[128];

    real_time*=1000;
    sys_time*=1000;
    user_time*=1000;

    sprintf(buff,strcat(header,"sys_time: %f ms, user_time: %f ms , real_time: %f ms \t"),sys_time,user_time,real_time);
    printf("%s\n",buff);
}

int is_numeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

int main(int argc,char** argv){

  int count, format;
  struct tms start_tms, end_tms;
  struct timespec start_ts, end_ts;

  if(strcmp(argv[1],"generate")==0){

    if(3<argc && (is_numeric(argv[3]) != 0)){
       count = atoi(argv[3]);
    }else{
      return -3;
    }

    if(4<argc && (is_numeric(argv[4]) != 0)){
      format = atoi(argv[4]);
    }else{
      return -4;
    }

    generate(count,format,argv[2]);

  }if(strcmp(argv[1],"sort")==0){

    if(set_mode(argv[5])!=0){
      return -5;
    }

    if(3<argc && (is_numeric(argv[3]) != 0)){
       count = atoi(argv[3]);
    }else{
      return -3;
    }

    if(4<argc && (is_numeric(argv[4]) != 0)){
      format = atoi(argv[4]);
    }else{
      return -4;
    }


    //time measure
    times(&start_tms);
    clock_gettime(CLOCK_REALTIME,&start_ts);

    sort(argv[2],count,format);

    times(&end_tms);
    clock_gettime(CLOCK_REALTIME,&end_ts);

    char buff[128];

    sprintf(buff,"%s %s count: %s format: %s %s",argv[1],argv[2],argv[3],argv[4],argv[5]);

    print_time(buff,&start_ts,&end_ts,&start_tms,&end_tms);


  }if(strcmp(argv[1],"copy")==0){

    if(set_mode(argv[6])!=0){
      return -6;
    }

    if(4<argc && (is_numeric(argv[4]) != 0)){
       count = atoi(argv[4]);
    }else{
      return -4;
    }

    if(5<argc && (is_numeric(argv[5]) != 0)){
      format = atoi(argv[5]);
    }else{
      return -5;
    }

    //
    times(&start_tms);
    clock_gettime(CLOCK_REALTIME,&start_ts);

    copy(argv[2],argv[3],count,format);

    times(&end_tms);
    clock_gettime(CLOCK_REALTIME,&end_ts);

    char buff[128];

    sprintf(buff,"%s %s %s count: %s format: %s %s",argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);
    print_time(buff,&start_ts,&end_ts,&start_tms,&end_tms);

  }

  return 0;
}
