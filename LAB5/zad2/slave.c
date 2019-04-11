#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char ** argv){
  srand(time(NULL));

  if(argc != 3){
    printf("WRONG ARGUMENT NUMB \n");
    printf("ARGUMENTS: [FIFO PATH], [NUMB OF LINES] \n");
    exit(-1);
  }

  int f = open(argv[1], O_RDWR);
  int num = atoi(argv[2]);

  if(num == 0){
    printf("WRONG NUMB OF LINES VALUE \n");
    printf("ARGUMENTS: [FIFO PATH], [NUMB OF LINES] \n");
    exit(-1);

  }

  if( f <= 0){
    printf("WRONG FIFO PATH \n");
    printf("ARGUMENTS: [FIFO PATH], [NUMB OF LINES] \n");
    exit(-1);
  }

  printf("SLAVE PID:%d\n",getpid());

  char date_buff[40];
  char final[45];

  while(num--){
    FILE * date = popen("date","r");
    fread(date_buff,sizeof(char),40,date);
    pclose(date);
    sprintf(final,"%s PID:%d",date_buff,getpid());
    write(f,final,sizeof(final));
    sleep((rand() % 5)+2);
  }
  close(f);
  return 0;
}
