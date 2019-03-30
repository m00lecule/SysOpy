#include "monitor.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv){
  mkdir("archiwum",0777);

  begin_monitoring(argv[1]);
  char buff[128];

  while(1){
     scanf ("%[^\n]%*c", buff);

    if(strcmp(buff,"LIST") ==0 ){
      printStack();
    } else if(strcmp(buff,"START ALL") == 0){
      start_all();
    }else if(strcmp(buff,"STOP ALL") == 0){
      stop_all();
    }else if(strcmp(buff,"END") == 0){
      end();
    }else if(strncmp(buff , "STOP",4) == 0){
      char* ptr;
      ptr = strtok(buff, " ");
      ptr = strtok(NULL, " ");
      pid_t pid = atoi(ptr);

      stop_pid(pid);
    }else if(strncmp(buff, "START",5) ==0 ){
      char * ptr;
      ptr = strtok(buff, " ");
      ptr = strtok(NULL, " ");
      pid_t pid = atoi(ptr);
      start_pid(pid);
    }
  }



  return 0;
}
