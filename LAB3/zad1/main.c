#include "stat_directory.h"

#ifndef _XOPEN_SOURCE
  #define _XOPEN_SOURCE 500
#endif

#include <sys/types.h>
#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char** argv){

  char* buff = realpath(argv[1],NULL);

  set_root(buff);
  search(buff);

  free(buff);

  return 0;
}
