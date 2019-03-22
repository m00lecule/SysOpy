#ifndef monitor_h
#define monitor_h

enum State{RAM=0,SYS=1};
void set_mode(enum State);
int begin_monitoring(const char * , unsigned int);
int file_monitoring(const char *, unsigned int, float);
int copy(const char *);
void wait_for_end();

#endif
