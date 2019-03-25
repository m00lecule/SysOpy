#include "monitor.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv){
  mkdir("archiwum",0777);

  if(argc !=4){
    printf("Wrong number of arguments \n");
    printf("arguments: [file] [runtime {s}] [mode {sys/ram}] \n");
    return -1;
  }

  char* end;
  int sec = strtol(argv[2],&end,10);

  if( sec < 0 ){
    printf("Wrong second argument \n");
    printf("arguments: [file] [runtime {s}] [mode {sys/ram}]\n");
    return -1;
  }

  if(strcmp("sys",argv[3])==0){
    set_mode(SYS);
  }else if(strcmp("ram",argv[3])==0){
    set_mode(RAM);
  }else{
    printf("Wrong third argument \n");
    printf("arguments: [file] [runtime {s}] [mode {sys/ram}]\n ");
    return -1;
  }

  begin_monitoring(argv[1],sec);
  wait_for_end();

  return 0;
}
