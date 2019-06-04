#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_WORDS 100
#define MAX_WORD_LEN 30

#include <netinet/in.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <pthread.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr_un sockaddr_un;
typedef struct epoll_event epoll_event;

typedef enum {
   REGISTER,
   REQUEST,
   RESPONSE,
   PING,
   FAILED
} message_type;

typedef struct message {
  message_type type;
  int id;
  int counter;
  char text[4096];
  char words[MAX_WORDS][MAX_WORD_LEN];
  int words_counter[MAX_WORDS];
} message;


int connect_unix(const char*);
int connect_inet(int,const char*);
int client_loop(int, const char*);
void exit_fun();
int sock_fd;
//char cli_name[15] = {"/tmp/"};

int main(int argc, char **argv)
{
  char *hostname, *address;

  atexit(exit_fun);
  //
  // sprintf(&cli_name[5],"%s",getpid());

  if(argc != 4 && argc != 5){
    printf("ARGS: [NAME] [INET/UNIX] [NAME / (PORT IP)]\n");
    return 1;
  }

  hostname = argv[1];
  address  = argv[3];
  if(strcmp(argv[2], "INET") == 0){
    int port = atoi(address);
    sock_fd = connect_inet(port,argv[4]);
  }else if(strcmp(argv[2], "UNIX") == 0){
    sock_fd = connect_unix(address);
    }else{
    printf("ARGS: [NAME] [INET/UNIX] [NAME / (PORT IP)]\n");
    return 1;
  }

   message msg;
   char delim[] =  " \n\r\t\0";

   msg.type = REGISTER;
   strcpy(msg.text, hostname);
   printf("%s\n",msg.text );
   send(sock_fd, &msg, sizeof(msg), 0);
   memset(&msg,0,sizeof(message));

   read(sock_fd,&msg,sizeof(msg));

   if(msg.type == FAILED){
     printf("Username already exists\n");
     exit(1);
   }else{
     printf("Registered to server\n");
   }

   int index;

   while(1){
     if(read(sock_fd, &msg, sizeof(msg)) < 0)
       { continue; }

     switch(msg.type)
     {
     case PING:
       break;

     case REQUEST:

       printf("GOT REQUEST\n");
       for(int i = 0 ; i < MAX_WORDS ; ++i){
         msg.words_counter[i]=0;
         msg.words[i][0]='\0';
       }

       printf("TEXT: %s\n",msg.text );

       msg.counter = 0;
       char* token;


       token = strtok(msg.text,delim);

       while(token!=NULL){
         ++msg.counter;

         index = -1;
         for(int i = 0 ; i < MAX_WORDS ; ++i){
           if(strcmp(msg.words[i],token) == 0){
             ++msg.words_counter[i];
             index = i;
             break;
           }
         }

         if(index == -1){
           for(int i = 0 ; i < MAX_WORDS ; ++i){
             if(msg.words_counter[i]==0){
               strcpy(msg.words[i],token);
               ++msg.words_counter[i];
               break;
             }
           }
         }

         token = strtok(NULL,delim);
       }

       printf("COUNTED WORDS %d \n",msg.counter);
       msg.type = RESPONSE;
       printf("TEXT: %s\n",msg.text );

       if(send(sock_fd, &msg, sizeof(message), 0) < 0)
         { printf("[SEND] ERROR\n"); }
       break;
     default:
       break;
     }

     memset(&msg, 0, sizeof(message));
   }

   close(sock_fd);
   return 0;
}

void exit_fun(){
  close(sock_fd);
}


int connect_unix(const char * sockpath)
{
  int sock_fd;
  sockaddr_un client_addr, server_addr;

  sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock_fd == -1)
  {
    printf("[UNIX] CREAT FAILURE\n");
    exit(2);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strcpy(server_addr.sun_path, sockpath);

  if ((connect(sock_fd, (sockaddr*) &server_addr, sizeof(server_addr))) != 0)
  {
    printf("[UNIX] CONNECTION FAILURE\n");
    exit(1);
  }

  return sock_fd;
}


int connect_inet(int port,const char * ip)
{
  int sock_fd;
  sockaddr_in server_addr;

  sock_fd = socket(AF_INET, SOCK_STREAM,0);
  if (sock_fd == -1){
     printf("[INET] CREAT FAILURE \n");
      exit(1);
    }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip);// htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  if (connect(sock_fd, (sockaddr*) &server_addr, sizeof(server_addr)) != 0){
     printf("[INET] CONNECTION FAILURE\n");
    exit(1); }

  return sock_fd;
}
