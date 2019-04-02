#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int received = 0;
static int send=0;
static volatile int condition=1;

void signal_sigusr1(int no, siginfo_t *info, void *ucontext){
  condition=0;
  received+=1;
  return;
}

void signal_sigusr2(int no, siginfo_t *info, void *ucontext){
  printf("send: %d received: %d\n",send,received );
  exit(0);
}




void send_all(pid_t pid,int mode){
  union sigval value;
    for(int i = 0 ; i < send; ++i){

      condition=1;
      if(mode == 0){
        if (kill(pid,SIGUSR1) != 0 )
         printf("ERROR WHILE SENDING SIGUSR1\n");
      }else if( mode == 1 ){
        if(sigqueue(pid, SIGUSR1, value) != 0)
          printf("ERROR WHILE SENDING SIGUSR1\n");
      }else if (mode == 2){
        if(kill(pid,SIGRTMIN + 12) != 0){}
      }
      sleep(1);

      if(condition == 1)
        printf("FOUND DEADLOCK\n" );
    }

    if(mode == 0){
       if (kill(pid,SIGUSR2) != 0 )
        printf("ERROR WHILE SENDING SIGUSR2\n");
    }else if( mode == 1){
      if(sigqueue(pid,SIGUSR2,value) != 0)
      printf("ERROR WHILE SENDING SIGUSR2\n");
    }else if( mode == 2){
      if(kill(pid,SIGRTMIN + 13) != 0)
      printf("ERROR WHILE SENDING SIGUSR2\n");
    }

}

int main(int argc, char** argv){
  if( argc != 4){
    printf("Wrong argument number\n");
    printf("[MODE] [PID] [COUNT]\n");
    return -1;
  }

  struct sigaction act;
 act.sa_flags = SA_SIGINFO;
 sigset_t mask;
 sigfillset(&mask);
 sigprocmask(SIG_SETMASK, &mask, NULL);

 sigset_t sigs;
 sigemptyset(&sigs);
 sigaddset(&sigs, SIGUSR2);
 sigaddset(&sigs, SIGUSR1);
 sigaddset(&sigs, SIGRTMIN+12);
 sigaddset(&sigs, SIGRTMIN+13);

 sigprocmask(SIG_UNBLOCK,&sigs,NULL);


 act.sa_sigaction = signal_sigusr1;
 sigfillset(&act.sa_mask);
 sigdelset(&act.sa_mask, SIGUSR1);
 sigdelset(&act.sa_mask, SIGRTMIN + 12);


 sigaction(SIGUSR1, &act, NULL);
 sigaction(SIGRTMIN + 12, &act, NULL);

 act.sa_sigaction = signal_sigusr2;
 sigfillset(&act.sa_mask);
 sigdelset(&act.sa_mask, SIGUSR2);
 sigdelset(&act.sa_mask, SIGRTMIN + 13);
 sigaction(SIGUSR2, &act, NULL);
 sigaction(SIGRTMIN + 13, &act, NULL);

 int mode = 0;

 if(strcmp(argv[1],"KILL") == 0)
  mode=0;
 else if(strcmp(argv[1],"SIGQUEUE") == 0)
  mode=1;
 else if( strcmp(argv[1],"SIGRT") == 0)
  mode =2;
else{
    printf("Wrong first argument\n");
    printf("[MODE] [PID] [COUNT]\n");
    return 0;
  }

  if((send = atoi(argv[3])) <= 0){
    printf("Wrong third argument\n");
    printf("[MODE] [PID] [COUNT]\n");
    return 0;
  }


  send_all(atoi(argv[2]),mode);

  return 0;
}
