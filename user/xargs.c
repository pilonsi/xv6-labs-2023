#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUFSZ 512

void 
resetargs(int *c, int *d, char *argv[]){
	// Takes an argv array with a current argc value of c and shrinks
	// it to a desired argc value of d
	for(int i=*c; i<*d; i++){
		free(argv[i]);
	}
}

int
main(int argc, char *argv[]){
	char *cargv[MAXARG];
	int cargc;
	char buf[BUFSZ];
	char b;
	int i;
	int child;

	if(argc < 2){
		printf("Usage: xargs command [args]\n");
		exit(1);
	}

	// Copy the tool to be invoked and its arguments to the new
	// argv vector
	for(i=1; i<argc; i++){
		cargv[i-1] = malloc(strlen(argv[i]) + 1);
		strcpy(cargv[i-1], argv[i]);
	}

	cargc = argc - 1;
	i = 0;

	// Keep reading lines from stdint separating them into args
	// & executing the tool
	while(read(0, &b, sizeof(char)) > 0){
	  if(i>=BUFSZ){
			fprintf(2, "xargs: Argument too long\n");
			exit(1);
		}

		if(b == '\n'){
			cargv[cargc] = malloc(i + 1);	
			memcpy(cargv[cargc], buf, i);
			*(cargv[cargc++]+i) = '\0';

			// Run the command and reset cargv
			child = fork();
			if(child==0){
				exec(cargv[0], cargv);
				fprintf(2, "xargs: exec %s failed\n", cargv[0]);
			}
			wait(&child);
			resetargs(&cargc, &argc, cargv);
			cargc = argc - 1;
			i = 0;
			
			continue;
		}
		
		if(strchr(" \t\v\r", b)){
			// End of argument. Append it to cargv
			cargv[cargc] = malloc(i + 1);	
			memcpy(cargv[cargc], buf, i);
			*(cargv[cargc++]+i) = '\0';
			i = 0;
			continue;
		}

		buf[i] = b;
		i++;			
	}

exit(0);
}
