#include "filehandler.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

extern int errno;

static char MODE[3] = {"sys"};

void generate(int count, int format, const char * file){

  int desc = open(file,O_WRONLY|O_CREAT,0666);
  char* buff2 = (char*)malloc(format*sizeof(char));

  for(int i=0 ; i<count ; ++i){
    for(int j = 0 ; j < format ; ++j){
        buff2[j] = (char)rand()%256;
    }
    write(desc,buff2,format);
  }

  free(buff2);
  close(desc);
}

int set_mode(const char * name){
  if(strcmp(name,"sys")!=0 && strcmp(name, "lib")!=0){
    return -1;
  }else{
    strcpy(MODE,name);
    return 0;
  }
}

int copy( const char * from, const char * to , int count, int format){

  char* buff = (char*)malloc(format);

  if(buff == NULL){
    return -2;
  }


  if(strcmp(MODE,"sys")==0){
    int desc_from = open(from,O_RDONLY);

    if(desc_from< 0)
      return -3;

    int desc_to = open(to,O_WRONLY|O_CREAT,0666);

    if(desc_to < 0 )
      return -4;


    for(int i=0;i<count;++i){
      read(desc_from,buff,format);
      write(desc_to,buff,format);
    }
    close(desc_from);
    close(desc_to);

  }else{
    FILE * desc_from = fopen(from,"r+");
    if(desc_from == NULL)
      return -5;

    FILE * desc_to = fopen(to,"w");

    if(desc_to==NULL)
      return -6;

    for(int i=0;i<count;++i){
      fread(buff,sizeof(char),format,desc_from);
      fwrite(buff,sizeof(char),format,desc_to);
    }

    fclose(desc_from);
    fclose(desc_to);
  }

  free(buff);
  return 0;
}

int sort( const char * file, int count ,int format){

  char * buff = calloc(format,sizeof(char));
  char * buff2 = calloc(format,sizeof(char));

  if(buff == NULL || buff2 == NULL)
    return -2;

  if(strcmp(MODE,"sys")==0){
    int desc = open(file,O_RDWR);
    int desc2 = open(file,O_RDWR);

    for(int i = 0 ; i < count - 1 ; ++i){
      read(desc,buff,format);
      lseek(desc2,(i+1)*format,SEEK_SET);

      for( int j = i + 1 ; j < count ; ++j){
        read(desc2,buff2,format);

        if(buff[0] < buff2[0]){
          lseek(desc2,-format,SEEK_CUR);
          write(desc2,buff,format);

          lseek(desc,-format,SEEK_CUR);
          write(desc,buff2,format);

          lseek(desc,-format,SEEK_CUR);
          read(desc,buff,format);
        }
      }
    }
    close(desc);
    close(desc2);
  }else{
    FILE * desc = fopen(file,"r+");
    FILE * desc2 = fopen(file,"r+");

    if( desc == NULL || desc2 == NULL)
      return -2;

    for(int i = 0 ; i < count - 1 ; ++i ){
      fread(buff,1,format,desc);
      fseek(desc2,(i+1)*format,0);

      for( int j = i + 1 ; j < count ; ++ j){
        fread(buff2,1,format,desc2);

        if(buff[0] < buff2[0]){
          fseek(desc2,-format,1);
          fwrite(buff,1,format,desc2);

          fseek(desc,-format,1);
          fwrite(buff2,1,format,desc);

          fseek(desc,-format,1);
          fread(buff,1,format,desc);
        }
      }
    }

    fclose(desc);
    fclose(desc2);
  }

  free(buff);
  free(buff2);

  return 0;
}
