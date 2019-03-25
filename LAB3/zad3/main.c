#include "monitor.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){
  mkdir("archiwum",0777);

  if(argc !=6){
    printf("Wrong number of arguments \n");
    printf("arguments: [file] [runtime {s}] [mode {sys/ram}] [memory limit {mb}] [cpu time limit {s}]\n ");
    return -1;
  }

  char* end;
  int sec = strtol(argv[2],&end,10);

  if(sec < 0 ){
    printf("Wrong second argument \n");
    printf("arguments: [file] [runtime {s}] [mode {sys/ram}] [memory limit {mb}] [cpu time limit {s}]\n ");
    return -1;
  }

  int limit_vm = strtol(argv[4],&end,10);

  if(limit_vm < 0 ){
    printf("Wrong third argument \n");
    printf("arguments: [file] [runtime {s}] [mode {sys/ram}] [memory limit {mb}] [cpu time limit {s}]\n ");
    return -1;
  }

  int limit_cpu= strtol(argv[5],&end,10);

  if(limit_cpu < 0 ){
    printf("Wrong fifth argument \n");
    printf("arguments: [file] [runtime {s}] [mode {sys/ram}] [memory limit {mb}] [cpu time limit {s}]\n ");
    return -1;
  }
  if(strcmp("sys",argv[3])==0){
    set_mode(SYS);
  }else if(strcmp("ram",argv[3])==0){
    set_mode(RAM);
  }else{
    printf("Wrong third argument \n");
    printf("arguments: [file] [runtime {s}] [mode {sys/ram}] [memory limit {mb}] [cpu time limit {s}]\n ");
    return -1;
  }

  begin_monitoring(argv[1],(unsigned int)sec,(unsigned int)limit_vm,(unsigned int)limit_cpu);
  wait_for_end();
  return 0;
}
