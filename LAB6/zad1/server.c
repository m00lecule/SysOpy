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
// #define CLIENT_LIMIT 100
//
// static struct client * client_tab[CLIENT_LIMIT];
//
// struct client {
//   int queue_id;
//   struct friend * next;
// } client;
//
// struct friend{
//   int queue_id;
//   struct friend * next;
// }
//
// client* init_client(int queue){
//   client * ret = (client*) malloc(sizeof(client));
//   ret->next=NULL;
//   ret->queue_id=queue;
//   return ret;
// }


struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} mesg_buffer;

//
// int process_message(struct mesg_buffer * msbf){
//   switch(msbf->mesg_type)
// }

char * random_string(size_t size){
  char * ptr =(char *) malloc(size*sizeof(char));

  for(int i = 0 ; i < size -1 ; ++i)
    ptr[i] = rand()%256;

  ptr[size-1] = '\0';

  return ptr;
}

int main(int argc, char** argv){

  srand(time(NULL));
  // for(int i = 0 ; i < CLIENT_LIMIT; ++i)
  //   client_tab[i]=NULL;
  const char *homedir;

  if ((homedir = getenv("HOME")) == NULL) {
      homedir = getpwuid(getuid())->pw_dir;
  }



  printf("%s\n",homedir );

  key_t key_own= ftok(homedir, 10);


  int queue_id = 0;


  if(( queue_id = msgget(key_own, 0777 | IPC_CREAT )) == -1 ){
    printf("exit\n" );
    exit(1);
  }

  printf("queue id%d\n",queue_id );
  struct mesg_buffer msgp;

  while(1){
    int bytes = msgrcv(queue_id,&msgp,sizeof(msgp.mesg_text),-4,MSG_NOERROR);

    printf("RECEIVED DATA:%s\n",msgp.mesg_text);
    long ret_id = atoi(msgp.mesg_text);

    sprintf(msgp.mesg_text,"AUTHORIZED BY SERVER");
    msgsnd(ret_id,&msgp,sizeof(msgp),0);
  }

  return 0;
}
