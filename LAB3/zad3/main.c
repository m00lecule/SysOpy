#include "monitor.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
  mkdir("archiwum",0666);
  char* end;
  int sec = strtol(argv[2],&end,10);

  if(end != NULL || sec < 0 ){
    printf("Wrong second argument \n");
    printf("arguments: [file] [runtime {s}] [cpu time limit {s}] [memory limit {mb}] [cpu time limit {s}]\n");
    return -1;
  }

  int limit_vm = strtol(argv[3],&end,10);

  if(end != NULL || limit_vm < 0 ){
    printf("Wrong second argument \n");
    printf("arguments: [file] [runtime {s}] [cpu time limit {s}] [memory limit {mb}] [cpu time limit {s}]\n");
    return -1;
  }

  int limit_cpu= strtol(argv[4],&end,10);

  if(end != NULL || limit_cpu < 0 ){
    printf("Wrong second argument \n");
    printf("arguments: [file] [runtime {s}] [memory limit {mb}] [cpu time limit {s}]\n");
    return -1;
  }

  set_mode(SYS);
  begin_monitoring(argv[1],(unsigned int)sec,(unsigned int)limit_vm,(unsigned int)limit_cpu);
  wait_for_end();
  return 0;
}
