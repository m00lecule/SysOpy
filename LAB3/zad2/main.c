#include "monitor.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv){
  mkdir("archiwum",0666);

  char* end;

  int sec = strtol(argv[2],&end,10);

  if(end != NULL || sec < 0 ){
    printf("Wrong second argument \n");
    printf("arguments: [file] [runtime {s}] ");
    return -1;
  }

  set_mode(SYS);

  begin_monitoring(argv[1],sec);
  wait_for_end();
  return 0;
}
