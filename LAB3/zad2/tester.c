#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void rand_string_set(char* ptr, size_t length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";

    if (length) {
            for (int n = 0;n < length;n++) {
                int key = rand() % (int)(sizeof(charset) -1);
                ptr[n] = charset[key];
            }

            ptr[length] = '\0';
        }
}

int main(int argc, char** argv){

  srand(time(NULL));

  int pmin = atoi(argv[2]);
  int pmax = atoi(argv[3]);
  size_t bytes = atoi(argv[4]);

  int freq = rand()%(pmax-pmin);
  freq += pmin;

  char * rand_string = calloc(sizeof(*rand_string),bytes+1);

  char buff[512];
  char buff_time[50];

  time_t curr_time;

  for(int i = 0 ; i < 10 ; ++i){
    int desc = open(argv[1],O_WRONLY|O_APPEND);
    if(desc < 0)
      return -1;

    sleep(freq);
    time(&curr_time);
    strftime(buff_time, 50, "_%Y-%m-%d_%H-%M-%S", localtime(&curr_time));
    rand_string_set(rand_string,bytes);
    sprintf(buff,"%d__%d__%s__%s\n",getpid(),rand(),buff_time,rand_string);
    write(desc,buff,strlen(buff));
    close(desc);
  }
  free(rand_string);

  return 0;
}
