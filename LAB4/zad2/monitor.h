#ifndef monitor_h
#define monitor_h


#include <sys/types.h>

struct Node{
  struct Node * next;
  pid_t pid;
  char path[128];
  u_int8_t interval;
};


struct Node* initNode(pid_t,char*,u_int8_t);
void freeStack();
void addToStack(struct Node*);
void printStack();


void begin_monitoring(const char *);
int file_monitoring(const char *, int);
void stop_pid(pid_t);
void stop_all();

void start_all();
void start_pid(pid_t);

void end();

int copy(const char *);
int prepare_static_variables(const char *);
void wait_for_end();

#endif
