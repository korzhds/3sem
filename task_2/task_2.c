
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

struct dpipe_t{
	int txd[2];
	int rxd[2];
};

int a=0;

int main() {
	struct dpipe_t fd;
	size_t size;
	int status;
	char buf_p[4096], buf_c[4096];
	char str_p[4096], str_c[4096];
	

	

if (pipe(fd.txd)<0){
    printf("Can\'t create pipe (txd)\n");
    return -1;
  }
  if (pipe(fd.rxd)<0){
    printf("Can\'t create pipe (rxd)\n");
    return -1;
  }
  
	printf("A message from parent to child:\n");
	fgets(str_p, 4095, stdin);
	printf("A message from child to parent: \n");
	fgets(str_c, 4095, stdin);
	
    pid_t pid = fork();
   if (pid < 0)
    {
    printf("Fork error\n");
    close(fd.txd[0]);
    close(fd.txd[1]);
    close(fd.rxd[0]);
    close(fd.rxd[1]);
    return -1;
  }
  
 
	
  
  if (pid > 0) {		//parent
	 
		size = write(fd.txd[1], str_p, strlen(str_p));
		if (size<0) {
			printf("Writing error\n");
        close(fd.rxd[0]);
        close(fd.txd[1]);
			exit(-1);
		}
		close(fd.txd[1]);

		waitpid(pid, &status, 0);

		size = read(fd.rxd[0], buf_p, strlen(str_c));
		
		if (size < 0){
        printf("Reading error\n");
        close(fd.rxd[0]);
        close(fd.txd[1]);
        exit(-1);
      }
      
      if (size == 0){
        close(fd.rxd[0]);
        close(fd.txd[1]);
        exit(0);
      }
		printf("I\'m a parent! My child sent: %s\n", buf_p);
		close(fd.rxd[0]);
	
 
}



	else {	//pid =0 , child
			
		size = read(fd.txd[0], buf_c, strlen(str_p));
		
		if (size < 0){
        printf("Reading error\n");
        close(fd.txd[0]);
        close(fd.rxd[1]);
        exit(-1);
      }
      
       if (size == 0){
        close(fd.txd[0]);
        close(fd.rxd[1]);
        exit(0);
      }
     
      
		printf("I\'m a child. My parent sent: %s\n", buf_c);
		close(fd.txd[0]);

		size = write(fd.rxd[1], str_c, strlen(str_c));
		if (size<0) {
			printf("Writing error\n");
			
			close(fd.rxd[1]);
            close(fd.txd[0]);
            exit(-1);
		}
		close(fd.rxd[1]);
		
	}


	return 0;

}


