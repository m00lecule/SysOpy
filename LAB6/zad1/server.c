#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <limits.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include "chat_headers.h"
#include <string.h>

#define CLIENT_NO 30

static struct mesg_buffer mesg;
static int queue_id;

static int friends[CLIENT_NO][CLIENT_NO];
static int client_list[CLIENT_NO];

void delete_client_friends_list(int client_id){

  if(client_id>=0 && client_id < CLIENT_NO)
    for(int i = 0 ; i < CLIENT_NO ; ++i)
      friends[client_id][i] = 0;
}


void print_friend_list(int client_id){
  printf("CLIENT FRIENDS: %d\n",client_id );

  for(int i = 0 ; i < CLIENT_NO ; ++i){
    if(friends[client_id][i]==1)
      printf("%d ",i);
    }


}

void list_clients(){
  char list[MESSAGE_SIZE];
  list[0] = '\0';
  char buff[14];

  for(int i = 0 ; i < CLIENT_NO; ++i)
    if(client_list[i] != -1){
      sprintf(buff,"%d ",i);
      strcat(list,buff);
    }

  strcpy(mesg.mesg_text,list);
}


void send_message_to_client(int client_id){

  if(client_id < CLIENT_NO && client_list[client_id] != -1){
    printf("RECEPIENT: %d \n",client_list[client_id]);
    msgsnd(client_list[client_id],&mesg,mesg_size(),0);
  }
}


int add_client(int queue_id){
  for( int i = 0 ; i < CLIENT_NO ; ++i)
    if(client_list[i] == -1){
      client_list[i]=queue_id;
      return i;
    }

  return -1;
}


void add_to_friend_list(int client_id,char* str){

  char * ptr = str;
  char * token = strtok_r(ptr," ",&ptr);
  while(ptr!=NULL && token!=NULL ){
    int index = strtol(token,NULL,10);
    if(index >=0 && index < CLIENT_NO){
      printf("added friend %d\n",index );
      friends[client_id][index] = 1;
    }

    token = strtok_r(ptr," ",&ptr);
  }
}

void delete_from_friend_list(int client_id, char* str){
  char * ptr = str;
  char * token = strtok_r(ptr," ",&ptr);

  while(ptr!=NULL && token!=NULL ){
    int index = strtol(token,NULL,10);

    if(index >=0 && index < CLIENT_NO)
      friends[client_id][index] = 0;

    token = strtok_r(ptr," ",&ptr);
  }
}

void init_handle(){
  short client_id;
  if( (client_id = add_client(mesg.id)) == -1){
    // how to handle this shit
  }

  printf("RECEIVED INIT FROM %d\n",client_id);
  mesg.type = INIT;
  mesg.priority=INIT_PRIOR;
  mesg.id = client_id;
  send_message_to_client(client_id);
}

void list_handle(){
  short client_id = mesg.id;
  printf("RECEIVED LIST FROM %d\n",mesg.id);
  list_clients();
  printf("%s\n",mesg.mesg_text );
  mesg.type = LIST;
  mesg.priority=LIST_PRIOR;
  send_message_to_client(client_id);
}

void all2_handle(){
  char buff[100];
  char buff2[14];
  printf("RECEIVED 2ALL FROM %d\n",mesg.id);
  sprintf(buff2," ID: %d :: ", mesg.id);

  time_t curr_time;
  time(&curr_time);
  strftime(buff, 100, "%Y-%m-%d_%H-%M-%S ", localtime(&curr_time));

  strcat(buff,buff2);
  strcat(buff,mesg.mesg_text);
  strcpy(mesg.mesg_text,buff);

  for(int i = 0 ; i < CLIENT_NO ; ++i)
    if(client_list[i]!= -1)
      send_message_to_client(i);

}

void friends2_handle(){
  char buff[100];
  char buff2[14];
  sprintf(buff2," ID: %d :: ", mesg.id);
  printf("RECEIVED 2FRIENDS FROM %d\n",mesg.id);
  time_t curr_time;
  time(&curr_time);
  strftime(buff, 100, "%Y-%m-%d_%H-%M-%S ", localtime(&curr_time));

  strcat(buff,buff2);
  strcat(buff,mesg.mesg_text);
  strcpy(mesg.mesg_text,buff);

  for(int i = 0 ; i < CLIENT_NO ; ++i)
    if(friends[mesg.id][i])
      send_message_to_client(i);
}


void add_handle(){
  printf("ADD HANDLE\n");
  add_to_friend_list(mesg.id,mesg.mesg_text);
  print_friend_list(mesg.id);
}

void del_handle(){
  printf("DELETE HANDLE\n");
  delete_from_friend_list(mesg.id,mesg.mesg_text);
  print_friend_list(mesg.id);
}

void stop_handle(){
  printf("RECEIVED STOP FROM %d\n",mesg.id);
  delete_client_friends_list(mesg.id);
  client_list[mesg.id]=-1;
}

void echo_handle(){
  char buff[100];
  time_t curr_time;
  time(&curr_time);
  printf("RECEIVED ECHO FROM %d\n",mesg.id);
  strftime(buff, 100, "%Y-%m-%d_%H-%M-%S ", localtime(&curr_time));
  strcat(buff,mesg.mesg_text);
  strcpy(mesg.mesg_text,buff);
  printf("SENDING TO %d\n",client_list[mesg.id]);
  send_message_to_client(mesg.id);
}

void one2_handle(){
  char buff[100];
  char buff2[14];
  time_t curr_time;
  time(&curr_time);
  printf("RECEIVED 2ONE FROM %d\n",mesg.id);
  strftime(buff, 100, "%Y-%m-%d_%H-%M-%S ", localtime(&curr_time));
  sprintf(buff2," ID: %d :: ", mesg.id);
  char* ptr = mesg.mesg_text;
  char* client = strtok_r(ptr," ",&ptr);
  short client_id = strtol(client,NULL,10);
  strcat(buff,buff2);
  strcat(buff,ptr);
  strcpy(mesg.mesg_text,buff);
  send_message_to_client(client_id);
}

void friends_handle(){
  delete_client_friends_list(mesg.id);
  if(mesg.mesg_text[0] != '\0'){
    add_to_friend_list(mesg.id,mesg.mesg_text);
  }
  print_friend_list(mesg.id);
}


void check_client_list(){
  for( int i = 0 ; i < CLIENT_NO ; ++i)
    printf("%d %d\n",i,client_list[i] );
}

void set_up_client_list(){
  for( int i = 0 ; i < CLIENT_NO ; ++i){
    client_list[i]= -1;

    for(int j = 0 ; j < CLIENT_NO ; ++j)
      friends[i][j]=0;
  }
}


void set_up_server_queue_id(){
  const char *homedir;

  if ((homedir = getenv("HOME")) == NULL) {
      homedir = getpwuid(getuid())->pw_dir;
  }

  key_t key_own= ftok(homedir, SERVER_SEED);

  if(( queue_id = msgget(key_own, 0777 | IPC_CREAT )) == -1 ){
    printf("exit\n" );
    exit(1);
  }
}
void exit_handle(){

  struct mesg_buffer exit_message;
  exit_message.priority = STOP_PRIOR;
  exit_message.type = STOP;

  for(int i = 0 ; i < CLIENT_NO ; ++i)
    if(client_list[i]!= -1){
      msgsnd(client_list[i],&exit_message,mesg_size(),0);
    }

  msgctl(queue_id, IPC_RMID, NULL);
  exit(0);
}

int main(int argc, char** argv){

  srand(time(NULL));
  setbuf(stdout, NULL);

  signal(SIGINT,exit_handle);
  set_up_client_list();
  set_up_server_queue_id();

  while(1){

    if(msgrcv(queue_id,&mesg,mesg_size(),0,IPC_NOWAIT) != -1){
      switch (mesg.type) {
        case INIT:
          init_handle();
          break;

        case LIST:
          list_handle();
          break;

        case ECHO:
          echo_handle();
          break;


        case ALL2:
          all2_handle();
          break;

        case FRIENDS2:
          friends2_handle();
          break;

        case ONE2:
          one2_handle();
          break;

        case FRIENDS:
          friends_handle();
          break;

        case STOP:
          stop_handle();
          break;

        case ADD:
          add_handle();
          break;

        case DEL:
          del_handle();
          break;

        default:
          break;
        }
      }
    }

  return 0;
}
