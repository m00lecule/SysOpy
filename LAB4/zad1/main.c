#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

pid_t child=0;

void sigtstp(){
  if(child == 0){
    child = fork();
    if(child == 0){
      execl("./date.sh", "date", NULL);
    }
  }else{
    printf("Oczekuje na CTRL+Z - kontynuacja; CTRL+C - zakonczenie\n");
    kill(child, SIGKILL);
    child = 0;
  }
}

void sigint(){
  printf("Odebrano sygnal SIGINT.\n");

  if(child != 0)
    kill(child, SIGKILL);

  exit(0);
}

int main(int argc, char** argv){

  struct sigaction act;
  act.sa_handler = sigtstp;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGTSTP, &act, NULL);

  signal(SIGINT, sigint);

  child = fork();
  if(child == 0){
    execl("./date.sh", "date", NULL);
  }


  while(1){}
  return 0;
}
