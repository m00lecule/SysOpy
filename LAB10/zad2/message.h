#ifndef MESSAGE_H
#define MESSAGE_H


  #define MAX_WORDS 100
  #define MAX_WORD_LEN 30

  typedef struct sockaddr sockaddr;
  typedef struct sockaddr_in sockaddr_in;
  typedef struct sockaddr_un sockaddr_un;
  typedef struct epoll_event epoll_event;

  typedef enum {
     REGISTER,
     REQUEST,
     RESPONSE,
     PING,
     FAILED,
     EXIT,
     PONG
  } message_type;

  typedef struct message {
    message_type type;
    int id;
    int counter;
    char text[4096];
    char words[MAX_WORDS][MAX_WORD_LEN];
    int words_counter[MAX_WORDS];
  } message;

#endif 
