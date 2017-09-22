/* Read the "4_exec_dir.c" carefully and correct the mistakes. Read about fork's return codes. */
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
 #include <stdlib.h>
#define  N 1000
int main() {
	pid_t pid;
   	int status;
   	int fin;
   //	char fin[N];
   	fin=0;
   	//char *fin1="quit";
   	//char *fin2;
   //	int compareresult;
   do	
{
   	switch(pid = fork()) {
   		case(-1):
   		    printf("error");
   		    exit(-1);
   		    
   		    case 0: 
			execl("/bin/ls","ls","-l",NULL); // this is the code the child runs 
   		break;
   		
   		
   		default: 
			wait(&status); // this is the code the parent runs
   	}
   	printf("if you want to exit, press '5' , else press another nuber\n");
   	scanf("%d",&fin);
}
//compareresult=srtcmp(fin1,fin2);
while(fin!=5);
	return 0;
}
