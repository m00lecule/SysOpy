#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv){

  if(argc != 2){
    printf("WRONG ARGUMENT NUMB\n");
    printf("ARGS: [FIFO PATH]\n");
    exit(-2);
  }

   int f = open(argv[1], O_RDWR);
   if(f < 0){
     printf("POTOK NAZWANY NIE ISTNIEJE, TWORZE NOWY\n" );
     if(mkfifo(argv[1], 0777) < 0){
       exit(-1);
     }
     f = open(argv[1], O_RDWR);
   }

   char buff[512];
   while(1){
    read(f, buff, 512);
    printf("%s\n",buff);
 }

  return 0;
}
