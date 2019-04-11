#include <sys/types.h>
#include <sys/ipc.h>
#include <limits.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>

struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} mesg_buffer;

char * random_string(size_t size){
  char * ptr =(char *) malloc(size*sizeof(char));

  for(int i = 0 ; i < size -1 ; ++i)
    ptr[i] = rand()%256;

  ptr[size-1] = '\0';

  return ptr;
}


int main(int argc, char** argv){

  srand(time(NULL));
   key_t key = atoi(argv[1]);

  char * rand_str = random_string(20);

  printf("PID:%d\n",getpid() );
  struct mesg_buffer msbf;
  msbf.mesg_type = 1;

  int queue_id = 0;

  // const char *homedir;
  //
  // if ((homedir = getenv("HOME")) == NULL) {
  //     homedir = getpwuid(getuid())->pw_dir;
  // }
  //
  // key_t key = ftok(homedir, 10);
  // printf("%d\n",key );

  if(( queue_id = msgget(IPC_PRIVATE, 0666 )) == -1 ){
    printf("exit\n" );
    exit(1);
  }


  msbf.mesg_type = 2;
  sprintf(msbf.mesg_text,"%d",queue_id);
  msgsnd(key,&msbf,sizeof(msbf),0);
  int bytes = msgrcv(queue_id,&msbf,sizeof(msbf.mesg_text),-4,MSG_NOERROR);
  printf("RECEIVED DATA FROM SERVER:%s\n",msbf.mesg_text);

  int times = 10;
  while(times --){
    sprintf(msbf.mesg_text,"CLIENT PID :%d",getpid());
    msgsnd(key,&msbf,sizeof(msbf),0);
    sleep(3);
  }

  free(rand_str);
  return 0;
}
