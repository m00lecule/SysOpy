 #define _GNU_SOURCE

 #include <stdio.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/wait.h>
 #include <string.h>
 #include <stdlib.h>


#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
 #define MAX_ARGS 5

void pipe_call(const char * first){
  int i = 0;
  int fd[4];

  char * ptr = strdup(first);
  char * theRest = ptr;
  char * token = strtok_r(theRest,"|",&theRest);
  char * next_token;
  char * token2 , * theRest2;
  char ** args = NULL;


  while(token){
    next_token = strtok_r(theRest,"|",&theRest);

    fd[0]=fd[2];
    fd[1]=fd[3];

    if(next_token){
      pipe(fd + 2 );
    }

    pid_t child = fork();
    if(child == 0){
      if( i != 0){
        close(fd[1]);
        dup2(fd[0],STDIN_FILENO);
      }

     close(fd[2]);

      if( next_token ){
        dup2(fd[3],STDOUT_FILENO);
      }

      if(args)
        free(args);
      args = calloc(MAX_ARGS + 2, sizeof(char*));
      int j = 0 ;
      theRest2 = token;
      char delimiters[3] = {' ','\n','\t'};

      while((token2 = strtok_r(theRest2,delimiters,&theRest2)))
        args[j++] = strdup(token2);

      args[j]=NULL;

      for( int i = 0 ; args[i] != NULL ; ++i)
        printf("%s:",args[i]);


      execvp(args[0],args);
      exit(0);

    }else{
      int status;
      close(fd[3]);
      waitpid(child,&status,0);
    }
    i++;
    token = next_token;
  }
}
int main(int argc, char** argv){

  FILE * f = fopen(argv[1],"r");

  if( f == NULL){
    printf("FILE DOESNT EXIST\n" );
    exit(1);
  }
  char* line;
  size_t size =0;

  while ( getline(&line, &size, f) != -1 ){
      printf("COMMAND:%s",line );
      pipe_call(line);
  }

  return 0;
}
