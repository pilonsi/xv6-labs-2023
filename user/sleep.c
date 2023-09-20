#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    printf("Error: you must give at least 1 argument\n");
    exit(1);
  } else {
    sleep(atoi(argv[1]));
    exit(0);
  }
}
