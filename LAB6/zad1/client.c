#define _GNU_SOURCE
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
#include "chat_headers.h"
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#define SIZE 1024

static struct mesg_buffer mesg;
static int server_queue_id = -1;
static int queue_id;
static int id;
static pid_t child = -1;
static int p[2];

char buff[1024];

void delete_queue(){
  msgctl(queue_id, IPC_RMID, NULL);
}

void send_message(){
  if(server_queue_id != -1){
    msgsnd(server_queue_id,&mesg,mesg_size(),0);
  }
}

void exit_function_parent(){
  close(p[0]);
  mesg.id = id;
  mesg.priority=STOP_PRIOR;
  mesg.type=STOP;
  send_message();
  delete_queue();

  if(child!=-1){
    kill(SIGKILL,child);
  }
    return;
}


void exit_function_child(){
  close(p[1]);

  return;
}

void set_up_server_queue_id(){
  const char *homedir;

  if ((homedir = getenv("HOME")) == NULL) {
      homedir = getpwuid(getuid())->pw_dir;
  }

  key_t key_own= ftok(homedir, SERVER_SEED);

  if(( server_queue_id = msgget(key_own, 0777 | IPC_CREAT )) == -1 ){
    exit(1);
  }
}


void set_up_own_queue(){
  if(( queue_id = msgget(IPC_PRIVATE, 0777 | IPC_CREAT )) == -1 ){
    exit(1);
  }
}

void init_mesg(){
  mesg.priority = INIT_PRIOR;
  printf("ququeid : %d\n",queue_id );
  mesg.id = queue_id;
  mesg.type = INIT;

  send_message();

  msgrcv(queue_id,&mesg,mesg_size(),0,0);

  if(mesg.id == -1){
    exit(-1);
  }

  id = mesg.id;
  printf("RECEIVED ID %d\n",id );
  fflush(stdout);
}

void exit_handle(){
  exit(0);
}

void parent_read(){
    close(p[1]);
    signal(SIGINT,exit_handle);
    atexit(exit_function_parent);

    set_up_server_queue_id();
    set_up_own_queue();
    init_mesg();

    while (1) {
        if(read(p[0], buff, SIZE) > 0){
            char* the_rest = buff;
            char delimiters[3] = {' ','\n','\t'};
            char * token = strtok_r(the_rest,delimiters,&the_rest);
            mesg.id = id;

            if(strcmp(token,"LIST") == 0 ){
              token = strtok_r(the_rest," ",&the_rest);
              mesg.priority = LIST_PRIOR;
              mesg.type = LIST;
              send_message();

            }else if(strcmp(token,"FRIENDS")==0){
              if(the_rest == NULL){
                mesg.mesg_text[0]='\0';
              }else{
                strcpy(mesg.mesg_text,the_rest);
              }
              mesg.priority = FRIENDS_PRIOR;
              mesg.type = FRIENDS;
              send_message();
            }else if(strcmp(token,"ADD")==0){
              if(strcmp(the_rest,"\0") == 0 ||strcmp(the_rest,"\n") == 0   ){
                printf("ADD COMMAND REQUIRES ARGUEMNTS\n");
              }else{
                strcpy(mesg.mesg_text,the_rest);
                mesg.priority = ADD_PRIOR;
                mesg.type = ADD;
                send_message();
              }

            }else if(strcmp(token,"DEL")==0){
              if(strcmp(the_rest,"\0") == 0 ||strcmp(the_rest,"\n") == 0   ){
                printf("DEL COMMAND REQUIRES ARGUEMNTS\n");
              }else{
                mesg.priority = DEL_PRIOR;
                mesg.type = DEL;
                strcpy(mesg.mesg_text,the_rest);
                send_message();
              }
            }else if(strcmp(token,"2ALL")==0){
              mesg.priority = ALL2_PRIOR;
              mesg.type = ALL2;

              strcpy(mesg.mesg_text,the_rest);
              send_message();
            }else if(strcmp(token,"2FRIENDS")==0){

              mesg.priority = FRIENDS2_PRIOR;
              mesg.type = FRIENDS2;
              strcpy(mesg.mesg_text,the_rest);
              send_message();
            }else if(strcmp(token,"2ONE")==0){

              mesg.priority = ONE2_PRIOR;
              mesg.type = ONE2;
              strcpy(mesg.mesg_text,the_rest);
              send_message();
            }else if(strcmp(token,"STOP")==0){
              exit(0);
            }else if(strcmp(token,"ECHO")==0){
              mesg.priority = ECHO_PRIOR;
              mesg.type = ECHO;
              strcpy(mesg.mesg_text,the_rest);
              send_message();
            }else{
              printf("MALFORMED INPUT: %s\n",buff);
              printf("COMMANDS: [ECHO string] [LIST] [FRIENDS clients_id_list] [2ALL string] [2FRIENDS string] [2ONE client_id string] [STOP] \n");
              fflush(stdout);
            }
        }

        if( msgrcv(queue_id,&mesg,mesg_size(),PRIORITY,IPC_NOWAIT) != -1 ){
          switch(mesg.type){
            case LIST :
              printf("LIST: %s\n",mesg.mesg_text);
              fflush(stdout);
            break;

            case ECHO:
              printf("ECHO: %s",mesg.mesg_text);
              fflush(stdout);
            break;

            case ALL2:
              printf("2ALL: %s",mesg.mesg_text);
              fflush(stdout);
            break;

            case ONE2:
              printf("2ONE: %s",mesg.mesg_text);
              fflush(stdout);
            break;

            case FRIENDS2:
              printf("2FRIENDS: %s",mesg.mesg_text );
              fflush(stdout);
            break;

            case STOP:
              exit(0);
            break;

            default:
            break;

          }
        }
    }
}

void child_write()
{
    close(p[0]);
    atexit(exit_function_child);
    while(1){
        fgets(buff,SIZE,stdin);

         char * ptr = strdup(buff);
         char * tmp = ptr;
         char * token = strtok_r(ptr," ",&ptr);

         if(strcmp(token,"READ") == 0){
           printf("%s\n",ptr );
           ptr[strcspn(ptr, "\n")] = 0;

           FILE * f = fopen(ptr,"r");
           if(f == NULL){
             printf("FILE DOESNT EXIST\n");
           }else{
             while (fgets(buff,sizeof(buff), f) != NULL) {

               write(p[1],buff,SIZE);
           }
             fclose(f);
           }
         }else{
          if(buff[0]!='\n')
            write(p[1], buff, SIZE);
        }
        free(tmp);
    }
}

int main(int argc, char** argv){

setbuf(stdout, NULL);

  if (pipe(p) < 0)
      exit(1);

  if (fcntl(p[0], F_SETFL, O_NONBLOCK) < 0)
      exit(2);

    switch (child = fork()) {
    case -1:
        exit(3);

    case 0:
        child_write();
        break;

    default:
        parent_read();
        break;
    }

  return 0;
}
