#ifndef headers_h
#define headers_h
#define STOP 12
#define LIST 11
#define GROUP 10
#define INIT 9
#define ECHO 8
#define LIST 7
#define FRIENDS 6
#define ADD 5
#define DEL 4
#define ALL2 3
#define ONE2 2
#define FRIENDS2 1
#define SERVERID 10

#define INIT_CLIENT
#define MESSAGE_SIZE 100

struct mesg_buffer {
    long mesg_type;
    char mesg_text[MESSAGE_SIZE];
} mesg_buffer;

#endif
