#ifndef headers_h
#define headers_h
#define STOP 12
#define STOP_PRIOR 4
#define LIST 11
#define LIST_PRIOR 3
#define INIT 10
#define INIT_PRIOR 1
#define ECHO 9
#define ECHO_PRIOR 1
#define FRIENDS 7
#define FRIENDS_PRIOR 2
#define ADD 6
#define ADD_PRIOR 1
#define DEL 5
#define DEL_PRIOR 1
#define ALL2 4
#define ALL2_PRIOR 1
#define ONE2 3
#define ONE2_PRIOR 1
#define FRIENDS2 2
#define FRIENDS2_PRIOR 1
#define PRIORITY -4
#define SERVER_SEED 10
#define MESSAGE_SIZE 100

#define SERVER_NAME "/server"

#define MESSAGE_SIZE 100

struct mesg_buffer {
    char mesg_text[100];
    int id;
    int type;
} mesg_buffer;


#endif
