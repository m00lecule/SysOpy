#include "monitor.h"
#include <unistd.h>

int main(int argc, char** argv){
  set_mode(SYS);
  begin_monitoring("mojplik.txt",100);
  wait_for_end();
  return 0;
}
