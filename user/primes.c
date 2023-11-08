#include "kernel/types.h"
#include "user/user.h"

#define STARTNO 2
#define ENDNO 35

#define STOP 0
#define RERUN 1

int
filter(int upp[2], int *child){
	int dnp[2];
	int n;
	int prime;

	// Close the write side of the upper pipe
	close(upp[1]);

	// Get the prime number from the parent
	if(read(upp[0], &prime, sizeof(int)) > 0) {
		printf("prime %d\n", prime);	
	} else {
		// No upstream pipe. Stop forking.
		exit(0);
	}

	// Setup the pipe for the child and fork
	pipe(dnp);

	*child = fork();	
	if(*child == 0) {
		// Close the unused pipes from parend and grandparent and 
		// setup the parent downpipe and rerun filter
		close(upp[0]);
		close(dnp[1]);
		upp[0] = dnp[0];
		upp[1] = dnp[1];
		return RERUN;
	} else {
		// Close the read side of the downpipe
		close(dnp[0]);

		// Read numbers from parent and filter
		while(read(upp[0], &n, sizeof(int)) > 0) {
			if(n % prime > 0) {
				write(dnp[1], &n, sizeof(int));
			} // Else drop it
		}
		
		// We are done. Close pipes
		close(dnp[1]);
		close(upp[0]);

		// Do not enter filter again. Wait for the child.
		return STOP;
	}
}

int
main(int argc, char *argv[]){  
  int child;
  int p[2];

  // Setup the 1st pipe, fork, send the number list and wait
  pipe(p);

  child = fork();
  if(child == 0){
		// Keep running the filter function which generates successive children
		// until there is no more numbers to filter
    while(filter(p, &child) == RERUN);
  } else {
		// The 1st parent just feeds the numbers
    close(p[0]);
    for(int i=STARTNO; i <= ENDNO; i++){
      write(p[1], &i, sizeof(int));
    }
    close(p[1]);
  }
  
  wait(&child);
  exit(0); 
}
