//#include "monitor.h"

int main(int argc, char** argv){
/*
  char line[128];
  char ptr[2][128];

  FILE * file = fopen("mojplik.txt","r+");

  if ( file != NULL )
  {
   char line [ 128 ];
   while ( fgets ( line, sizeof line, file ) != NULL )
   {
      printf("%s",line );
      char * pch = strtok (line," ,.-");

      int counter=0;
      while (pch != NULL)
      {
        printf ("%s\n",pch);
        strcpy(ptr[counter++],pch);
        pch = strtok (NULL, " ,.-");
      }
   }
   fclose ( file );
 }


printf("%s\n",ptr[0] );
printf("%s\n",ptr[1] );

 int status;
 pid_t childPid = fork();

 if(childPid>0){
   waitpid(childPid,&status,0);
   printf("%d\n", WEXITSTATUS(status));
 }else{
   sleep(2);

   int i=0;

   struct stat st;

   file = fopen("mojplik.txt","r+");

   stat("mojplik.txt",&st);

   printf("%ld\n", st.st_mtime);
   printf("%ld\n", st.st_atime);
   printf("%ld\n", st.st_ctime);
   char* buff = "xd";
   fwrite(buff,sizeof(char),strlen(buff),file);
   fclose(file);

   strcpy(ptr[1],"123");

   sleep(1);
   stat("mojplik.txt",&st);
   printf("%ld\n", st.st_mtime);
   printf("%ld\n", st.st_atime);
   printf("%ld\n", st.st_ctime);

   printf("%s\n",ptr[1] );
   for(i;i<30;++i){}

   printf("%d\n",i );
   return i;
 }

 printf("%s\n",ptr[1] );
*/

int michal =1;
printf("%d\n",michal );
  return 0;
}
