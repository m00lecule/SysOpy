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

typedef struct list{
  struct list * next;
  int index;
}list;


typedef struct client{
  int queue_id;
  struct list * friends;
}client;


static client client_list[CLIENT_NO];

void delete_client_friends_list(int client_id){
  struct list * ptr;
  struct list * next;

  ptr = client_list[client_id].friends;
  next = ptr->next;

  do{
    free(ptr);
    ptr = next;
    next = next->next;
  }while(next != NULL);

  client_list[client_id].friends=NULL;
}


void print_friend_list(int client_id){
  struct list * ptr = client_list[client_id].friends;
  printf("CLIENT FRIENDS: %d\n",client_id );
  while(ptr!=NULL){
    printf("%d\n",ptr->index );
    ptr=ptr->next;
  }
}

void list_clients(){
  char list[MESSAGE_SIZE];
  list[0] = '\0';
  char buff[14];

  for(int i = 0 ; i < CLIENT_NO; ++i)
    if(client_list[i].queue_id != -1){
      sprintf(buff,"%d ",i);
      strcat(list,buff);
    }

  strcpy(mesg.mesg_text,list);
}


void send_message_to_client(int client_id){

  if(client_id < CLIENT_NO && client_list[client_id].queue_id != -1){
    printf("RECEPIENT: %d \n",client_list[client_id].queue_id );
    msgsnd(client_list[client_id].queue_id,&mesg,mesg_size(),0);
  }
}


int add_client(int queue_id){
  for( int i = 0 ; i < CLIENT_NO ; ++i)
    if(client_list[i].queue_id == -1){
      client_list[i].queue_id=queue_id;
      return i;
    }

  return -1;
}

struct list * string_to_list(char * str){
  struct list * before;
  struct list * after;

  char* the_rest = str;
  char* token = strtok_r(the_rest," ",&the_rest);

  after = calloc(1,sizeof(list));
  after->next=NULL;
  after->index = strtol(token,NULL,10);
  char delimiters[3] = {' ','\n','\t'};
  token = strtok_r(the_rest,delimiters, &the_rest);
  while( token != NULL && the_rest!=NULL){
    before = calloc(1,sizeof(list));
    before->index = strtol(token,NULL,10);
    before->next=after;
    after=before;
    token = strtok_r(the_rest,delimiters, &the_rest);
  }
  return after;
}


void add_to_friend_list(int client_id,char* str){

  struct list * friends = string_to_list(str);
  struct list * temp = NULL;
  struct list * after;
  short cond;
  if(client_list[client_id].friends == NULL){
    client_list[client_id].friends=friends;
  }else{


    while(friends != NULL){
      temp = client_list[client_id].friends;
      cond = 0;
      after->next = client_list[client_id].friends;
      while(temp!=NULL){
        if(temp->index == friends->index){
          temp = friends;
          friends = friends->next;
          free(temp);
          cond = 1;
          break;
        }
        temp=temp->next;
        after= after->next;
      }

      if(cond == 0){
        after->next = friends;
        friends = friends -> next;
        after->next->next=NULL;
      }
    }
  }
}

void delete_from_friend_list(int client_id, char* str){
  struct list * friends = string_to_list(str);
  struct list * temp;
  struct list * del;
  short cond = 0;
  while(friends!=NULL){
    cond = 0;
    temp = client_list[client_id].friends;
    if(temp->index == friends->index){
      client_list[client_id].friends = client_list[client_id].friends->next;
      continue;
    }
    while(temp->next!=NULL){
      if(temp->next->index == friends->index){
        del = temp->next;
        temp->next = del->next;
        free(del);
        del = friends;
        friends = friends->next;
        free(del);
        cond = 1;
        break;
      }
      temp=temp->next;
    }

    if(cond == 0 ){
      del = friends;
      friends = friends->next;
      free(del);
    }
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
    if(client_list[i].queue_id != -1)
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

  struct list * it = client_list[mesg.id].friends;

  while(it != NULL){
    send_message_to_client(it->index);
    it = it->next;
  }
}


void add_handle(){
  add_to_friend_list(mesg.id,mesg.mesg_text);
  print_friend_list(mesg.id);
}

void del_handle(){
  delete_from_friend_list(mesg.id,mesg.mesg_text);
  print_friend_list(mesg.id);
}

void stop_handle(){
  printf("RECEIVED STOP FROM %d\n",mesg.id);
  delete_client_friends_list(mesg.id);
  client_list[mesg.id].queue_id=-1;
}

void echo_handle(){
  char buff[100];
  time_t curr_time;
  time(&curr_time);
  printf("RECEIVED ECHO FROM %d\n",mesg.id);
  strftime(buff, 100, "%Y-%m-%d_%H-%M-%S ", localtime(&curr_time));
  strcat(buff,mesg.mesg_text);
  strcpy(mesg.mesg_text,buff);
  printf("SENDING TO %d\n",client_list[mesg.id].queue_id );
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
  if(mesg.mesg_text[0] == '\0'){
    delete_client_friends_list(mesg.id);
  }else{
    struct list* friends = string_to_list(mesg.mesg_text);
    if(client_list[mesg.id].friends != NULL)
      delete_client_friends_list(mesg.id);

  client_list[mesg.id].friends = friends;
  }
  print_friend_list(mesg.id);
}


void check_client_list(){
  for( int i = 0 ; i < CLIENT_NO ; ++i)
    printf("%d %d\n",i,client_list[i].queue_id );
}

void set_up_client_list(){
  for( int i = 0 ; i < CLIENT_NO ; ++i){
    client_list[i].friends = NULL;
    client_list[i].queue_id=-1;
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
    if(client_list[i].queue_id != -1){
      msgsnd(client_list[i].queue_id,&exit_message,mesg_size(),0);
    }

  msgctl(queue_id, IPC_RMID, NULL);
  exit(0);
}

int main(int argc, char** argv){

  srand(time(NULL));

  signal(SIGINT,exit_handle);
  set_up_client_list();
  set_up_server_queue_id();

  while(1){

    if(msgrcv(queue_id,&mesg,mesg_size(),0,0) != -1){

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
