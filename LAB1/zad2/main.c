#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#define BILLION 1000000000

#ifdef DYNAMIC
#include <dlfcn.h>
typedef struct MemoryArray{
  char** blk;
  int size;
}MemoryArray;
#endif

FILE * file = NULL;

int is_numeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

#ifndef DYNAMIC
#include "findResultLib.h"
#endif

#ifdef DYNAMIC
  struct MemoryArray* (*init_memory_array)(int);
  void (*set_filename)(const char *);
  void (*set_directory)(const char *);
  int (*fetch_from_tmp)(struct MemoryArray*, const char*);
  void (*exec_find)(const char*);
  void (*reset_index)(struct MemoryArray*, int);
  void (*free_memory)(struct MemoryArray*);

  void load_library(){
    void *handle = dlopen("libfindresult.so",RTLD_LAZY);
    if(handle == NULL){
      exit(1);
    }

    init_memory_array = (struct MemoryArray* (*) (int))dlsym(handle,"init_memory_array");
    set_filename = (void (*) (const char *))dlsym(handle,"set_filename");
    set_directory = (void (*)(const char *))dlsym(handle,"set_directory");
    fetch_from_tmp = (int (*)(struct MemoryArray*,const char*))dlsym(handle,"fetch_from_tmp");
    exec_find = (void (*)(const char*))dlsym(handle,"exec_find");
    reset_index = (void (*)(struct MemoryArray*, int))dlsym(handle,"reset_index");
    free_memory = (void (*)(struct MemoryArray* ))dlsym(handle,"free_memory");

    if(dlerror() != NULL){
        exit(1);
    }
  }
#endif

void print_error(const char * str){
  printf("Wrong argument: %s\n", str);
  printf("List of arguments:\n - create_table (int)size \n - search_directory (text)root_directory (text)filename (text)output temporary filename \n - remove_index (unsigned int)index \n - add_to_memory (text)output temporary filename\n ");
  if(file != NULL){
    fclose(file);
  }
}


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
    fprintf(file, "%s\n",buff );
}

int main(int argc, char** argv){

  #ifdef DYNAMIC
    load_library();
  #endif

  file = fopen(argv[1], argv[2]);

  if(file == NULL){
	printf("Wrong raport file or open flag\n");
	return -2;
}

  int i=3;
  struct MemoryArray * ptr = NULL;
  struct tms start_tms, end_tms;
  struct timespec start_ts, end_ts;


    while(i<argc){
    times(&start_tms);
    clock_gettime(CLOCK_REALTIME,&start_ts);

      if(strcmp(argv[i],"create_table")==0){

        free_memory(ptr);

        int size=0;
        if(i+1<argc && (is_numeric(argv[i+1]) != 0)){
          size = atoi(argv[i+1]);
        }
        else{
          print_error(argv[i+1]);
          return -1;
        }

        ptr=init_memory_array(size);

        if(ptr==NULL){
          return -5;
        }

        times(&end_tms);
        clock_gettime(CLOCK_REALTIME,&end_ts);

        char buff[128];

        sprintf(buff,"create_table %s ",argv[i+1]);

        print_time(buff,&start_ts,&end_ts,&start_tms,&end_tms);

        i+=2;
      }else if(strcmp(argv[i],"search_directory")==0){

        if(i+3>argc){
          print_error(argv[i]);
          free_memory(ptr);
          return -5;
        }

        set_filename(argv[i+2]);
        set_directory(argv[i+1]);
        exec_find(argv[i+3]);

        times(&end_tms);
        clock_gettime(CLOCK_REALTIME,&end_ts);

        char buff[128];

        sprintf(buff,"search_directory %s %s ",argv[i+1],argv[i+2]);

        print_time(buff,&start_ts,&end_ts,&start_tms,&end_tms);
          i+=4;
      }else if(strcmp(argv[i],"remove_block") == 0){

        int index=0;
        if(i+1<argc && (is_numeric(argv[i+1]) != 0))
          index = atoi(argv[i+1]);
        else{
          print_error(argv[i+1]);
          free_memory(ptr);
          return -2;
        }
        char buff[128];

        if(ptr->blk[index]!=NULL){
          sprintf(buff,"remove_block %s, size: %ld ",argv[i+1],strlen(ptr->blk[index])*sizeof(char));
        }
        else{
          sprintf(buff,"remove_block %s ",argv[i+1]);
        }

        reset_index(ptr,index);
        times(&end_tms);
        clock_gettime(CLOCK_REALTIME,&end_ts);

        print_time(buff,&start_ts,&end_ts,&start_tms,&end_tms);
        i+=2;

      }else if(strcmp(argv[i],"add_to_memory")==0){

        if(i+1>=argc){
          print_error(argv[i]);
          free_memory(ptr);
          return -2;
        }

        int index = fetch_from_tmp(ptr,argv[i+1]);

        times(&end_tms);
        clock_gettime(CLOCK_REALTIME,&end_ts);

        char buff[128];
        if(index >=0)
          sprintf(buff,"add_to_memory %s.tmp size: %ld ",argv[i+1],strlen(ptr->blk[index])*sizeof(char));
        else
          strcpy(buff,"add_to_memory failed");

        print_time(buff,&start_ts,&end_ts,&start_tms,&end_tms);
        i+=2;
      }else{
        char buff[] = "unrecognized argument";
        print_error(buff);
        free_memory(ptr);
        return -2;
      }
    }

    free_memory(ptr);
    fclose(file);
  return 0;
}
