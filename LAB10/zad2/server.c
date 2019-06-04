#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <signal.h>
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

static pid_t child = -1;
static int p[2];
#define UNIX_PATH_MAX    108
#define MAX_CLIENTS      16
#define MAX_EPOLL_EVENTS 128
#define USERNAME_MAX_LEN 30


void *listener_thread(void *);
void *input_thread(void *);
void handle_event(epoll_event*);
void child_write();

void exit_handler(int);
void exit_handler_child();
int register_client(int, const char*);

static volatile int RUNNING = 1;

static int PORT;
static char UNIX_PATH[UNIX_PATH_MAX];

static int client_fd[MAX_CLIENTS] = {};
static int client_state[MAX_CLIENTS] = {};
static char client_name[MAX_CLIENTS][USERNAME_MAX_LEN];
static sockaddr client_address[MAX_CLIENTS];

static int request = 0;

int main(int argc, char **argv)
{
  pthread_t listener_tid, input_tid;

  if(argc != 3){
    printf("ARGS [PORT] [UNIX PORT NAME]\n");
    fflush(stdout);
    return 1;
  }

  PORT = atoi(argv[1]);
  strcpy(UNIX_PATH, argv[2]);
  signal(SIGINT, exit_handler);

  if(pthread_create(&listener_tid, NULL, listener_thread, NULL) != 0){
    return 1;
  }

  if(pthread_create(&input_tid, NULL, input_thread, NULL) != 0){
    return 1;
  }


  if(pthread_join(listener_tid, NULL) != 0){
    return 1;
  }

  if(pthread_join(input_tid, NULL) != 0){
    return 1;
  }

  return 0;
}

void handle_event(epoll_event *event)
{
  message msg;
  memset(&msg,0, sizeof(msg));

  size_t bytes_read;
  int fd;
  sockaddr addr;
  socklen_t len;
  int index = 0;

  fd = event->data.fd;
  len = sizeof(addr);
  bytes_read = recvfrom(fd, &msg, sizeof(message),0,&addr,&len);

  if(bytes_read == sizeof(message))
  {
    switch(msg.type)
    {
    case REGISTER:
      if( (index = register_client(fd, msg.text)) >= 0){
        memset(&msg, 0 , sizeof(message));
        msg.type=RESPONSE;
        client_address[index] = addr;
        if(sendto(fd, &msg, sizeof(msg),0 , &addr,sizeof(addr)) < 0){
          printf("Couldnt send message to client\n" );
          fflush(stdout);
        }

        printf("REGISTERED CLIENT %s\n",client_name[index]);
        fflush(stdout);
      }else{
        memset(&msg, 0 , sizeof(message));
        msg.type=FAILED;
        if(sendto(fd, &msg, sizeof(msg), 0 , &addr, sizeof(addr)) < 0){
          printf("Couldnt send message to client\n" );
          fflush(stdout);
        }
      }

      break;
    case RESPONSE:
      printf("RESPONSE %d\n", msg.id);
      printf("Words: %d\n", msg.counter);


      fflush(stdout);
      for(int i = 0 ; i < MAX_WORDS ; ++i){
          if(msg.words_counter[i] == 0){
          break;
          }


        printf("%s : %d \n",msg.words[i],msg.words_counter[i] );
        fflush(stdout);
      }

      for(int i=0; i < MAX_CLIENTS; i++)
      {
        if(client_fd[i] == fd)
          {
            printf("--------------- \nFROM:%s\n",client_name[i]);
            client_state[i] = 0;
            break; }
      }

      break;
    default:
      break;
    }
  }
}


void *listener_thread(void *arg)
{
  int inet_fd, unix_fd, cli_fd;
  sockaddr_in inet_addr, inet_cli;
  sockaddr_un unix_addr, unix_cli;
  socklen_t addr_len = sizeof(sockaddr);
  int flags;

  int epoll_fd, event_count;
  epoll_event events[MAX_EPOLL_EVENTS];

  if((inet_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
    printf("[INET] CREAT FAILURE\n");
    fflush(stdout);
    exit(1);
  }

  flags = fcntl(inet_fd, F_GETFL);
  fcntl(inet_fd, F_SETFL, flags | O_NONBLOCK);

  memset(&inet_addr, 0, sizeof(inet_addr));
  inet_addr.sin_family = AF_INET;
  inet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  inet_addr.sin_port = htons(PORT);

  if ((bind(inet_fd, (sockaddr*) &inet_addr, sizeof(inet_addr))) != 0){
    printf("[INET] BIND FAILURE\n");
    fflush(stdout);
    exit(2);
  }

  unix_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (unix_fd == -1){
      printf("[UNIX] CREAT FAILURE\n");
      fflush(stdout);
      exit(2);
  }

  flags = fcntl(unix_fd, F_GETFL);
  fcntl(unix_fd, F_SETFL, flags | O_NONBLOCK);

  memset(&unix_addr, 0, sizeof(unix_addr));
  unix_addr.sun_family = AF_UNIX;
  strcpy(unix_addr.sun_path, UNIX_PATH);

  if ((bind(unix_fd, (sockaddr*) &unix_addr, sizeof(unix_addr))) != 0){
    printf("[UNIX] BIND FAILURE\n");
    fflush(stdout);
    exit(1);
  }

  if((epoll_fd = epoll_create1(0)) == -1){
    printf("[EPOLL] CREAT FAILURE\n");
    fflush(stdout);
    exit(1);
  }

   epoll_event event;
   event.events = EPOLLIN;
   event.data.fd = unix_fd;

   if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, unix_fd, &event) < 0){
     printf("[EPOLL] ADD FAILURE\n");
     exit(1);
   }

  event.data.fd = inet_fd;

  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, inet_fd, &event) < 0){
    printf("[EPOLL] ADD FAILURE\n");
    exit(1);
  }

  message msg_ping;

  memset(&msg_ping, 0, sizeof(msg_ping));
  msg_ping.type = PING;


  while(RUNNING){
    event_count = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, 0);

    for(int i = 0; i < event_count; i++)
      handle_event(&events[i]);

    // for(int i=0; i<MAX_CLIENTS; i++)
    // {
    //   if(client_fd[i] == 0)
    //     continue;
    //
    //   msg_ping.type = PING;
    //   if(sendto(client_fd[i], &msg_ping, sizeof(msg_ping), 0 , client_addr[i],sizeof(sockaddr)) < 0)
    //   {
    //     printf("CLIENT: %s IS NOT RESPONDING\n", client_name[i]);
    //     fflush(stdout);
    //     client_fd[i] = client_state[i] = 0;
    //     client_name[i][0] = '\0';
    //   }
    // }
  }

  close(inet_fd);
  close(unix_fd);
  close(epoll_fd);
  unlink(UNIX_PATH);

  return NULL;
}

void *input_thread(void *arg)
{
  message msg;
  char path[UNIX_PATH_MAX];
  int fp;

  memset(&msg, 0, sizeof(message));
  msg.type = REQUEST;

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
        break;
    }

  while(RUNNING)
  {
    if(read(p[0],path,sizeof(path)) > 0){
      fp = open(path, O_RDONLY);

      if(fp < 0){
          continue;
      }

      if(read(fp, msg.text, sizeof(msg.text)) < 0){
          continue;
      }

      msg.id = request++;

      int cli_idx, cli_fd;
      cli_idx = -1;

      for(int i=0; i<MAX_CLIENTS; i++){
        if(client_fd[i] == 0)
          { continue; }

        cli_idx = i;
        if(!client_state[i])
          { break; }
      }

      cli_fd = client_fd[cli_idx];

      if(cli_idx == -1 || cli_fd == 0){
         printf("No availablie clients\n");
         fflush(stdout);
         continue;
       }
       printf("sending: %s\n",msg.text);

      if(sendto(cli_fd, &msg, sizeof(msg), 0 , &client_address[cli_idx], sizeof(sockaddr)) < 0){
         printf("[SENDING] TO %d ERROR\n", cli_fd);
         fflush(stdout);
      }
      else{
        client_state[cli_idx] = 1;
      }
    }
  }

  return NULL;
}

void exit_handler(int sig)
{
  close(p[0]);

  if(child!=-1){
    kill(SIGKILL,child);
  }
  RUNNING = 0;
}


int register_client(int fd, const char *hostname)
{

  for( int i = 0 ; i < MAX_CLIENTS ; ++i){
    if(strcmp(client_name[i],hostname) == 0 )
      return -1;
  }

  for( int i = 0 ; i < MAX_CLIENTS ; ++i){
    if(client_fd[i] ==0){
      client_fd[i] = fd;
      client_state[i] = 0;
      strcpy(client_name[i], hostname);
      return i;
    }
  }

  return -2;
}

void exit_handler_child(){
  close(p[1]);

  return;
}

void child_write()
{
    close(p[0]);
    char buff[512];
    atexit(exit_handler_child);
    while(1){
        scanf("%s\n",buff );
          if(buff[0]!='\n'){
            write(p[1], buff, sizeof(buff));
          }
    }
}
