#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "findResultLib.h"

//przeszukiwany katalog
static char* dir = NULL;
//poszukiwany plik
static char* filename = NULL;

MemoryArray* init_memory_array(int size){
  if(size<=0)
    return NULL;

  MemoryArray* res = (MemoryArray *)calloc(1,sizeof(MemoryArray));

  if(res == NULL)
    return NULL;

  res->blk = (char**)calloc(size,sizeof(char*));

  if(res->blk == NULL){
    free(res);
    return NULL;
  }

  res->size=size;

  for(int i=0;i<size;++i){
    res->blk[i]=NULL;
  }

  return res;
}

void set_filename(const char * fn){
  if(filename!=NULL)
    free(filename);

  int size = strlen(fn);

  filename = (char*)calloc(size,sizeof(char));

  if(filename != NULL)
    strcpy(filename,fn);
}

void set_directory(const char * dr){
  if(dir!=NULL)
    free(dir);

  int size=strlen(dr);

  dir=(char*)calloc(size,sizeof(char));

  if(dir != NULL)
    strcpy(dir,dr);
}

int fetch_from_tmp(MemoryArray* frt,const char* temp_file){

  if(frt==NULL)
    return -4;

  for(int i=0;i<frt->size;++i)
    if(frt->blk[i]==NULL){

      char buff[128];
      sprintf(buff,"%s.tmp",temp_file);

      FILE *f = fopen(buff, "r");
       if(f==NULL){
           return -2;
         }

      fseek(f, 0, SEEK_END);
      int size=ftell(f);

      frt->blk[i] = (char*)calloc(size,sizeof(char));
      fseek(f,0,SEEK_SET);
      fread(frt->blk[i],sizeof(char),size,f);
      fclose(f);
      return i;
  }
  return -1;
}

void exec_find(const char* temp_file){
  char buff[128];

  if(filename==NULL){
    fprintf(stderr,"brak nazwy pliku");
    return;
  }

  if(dir == NULL){
    fprintf(stderr,"brak nazwy katalogu");
    return;
  }

  sprintf(buff,"find %s -name %s 1>%s.tmp 2>/dev/null",dir,filename,temp_file);
  system(buff);
}

void reset_index(MemoryArray* frt, int index){
    if(frt==NULL)
      return;

    if(index >= 0 && index<frt->size && frt->blk[index]!=NULL){
      free(frt->blk[index]);
      frt->blk[index]=NULL;
    }
}

void free_memory(MemoryArray* ptr){

  if(ptr==NULL){
    return;
  }

    if(ptr->blk!=NULL){
      for(int i = 0 ; i<ptr->size ; ++i){
        if(ptr->blk[i]!=NULL){
          free(ptr->blk[i]);
        }
      }
        free(ptr->blk);
    }

    free(ptr);
    ptr=NULL;
}
