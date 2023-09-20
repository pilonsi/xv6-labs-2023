#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
  int p[2];
  char b = 0;
  char ping = 100;
  char pong = 200;

  pipe(p);
  int pid;

  if (fork() == 0) {
    pid = getpid();
    read(p[0], &b, 1);
    if (b == ping) {
      printf("%d: received ping\n", pid);
      close(p[0]);
      write(p[1], &pong, 1); 
      close(p[1]);
      exit(0);
    } else {
      close(p[0]);
      close(p[1]);
      printf("%d: received unexpected %s\n", pid, b);
      exit(1);
    }
  } else {
    pid = getpid();
    write(p[1], &ping, 1);
    close(p[1]);
    read(p[0], &b, 1);
    if (b == pong) {
      printf("%d: received pong\n", pid);
      close(p[0]);
      exit(0);
    } else {
      close(p[0]);
      printf("%d: received unexpected %s\n", pid, b);
      exit(1);
    }
  };
}
