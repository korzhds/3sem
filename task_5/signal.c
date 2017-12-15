/* По материалам https://habrahabr.ru/post/122823/ и проч.*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define EXIT_SUCCESS 0
#define EXIT_FALURE -1

char *in_pathname, *out_pathname;
pid_t chd_id = 0, par_id = 0;
int in_fd = 0, out_fd = 0;

void sigterm_handler(int code) {
	printf("TERM signal received\n");
	exit(-1);
}

void sig_chld_handler(int code) {
	printf("CHLD signal received\n");
}

void usr1_handler(int code) {	//ready signal
	in_fd = open(in_pathname, O_RDONLY);
	if (in_fd == -1) {
		printf("Unable to open input file\n");
		exit(EXIT_FALURE);
	}

	char c;
	int retval = read(in_fd, (void *) &c, 1);
	if (retval == -1) {
		printf("Unable to read from input fd\n");
		exit(EXIT_FALURE);
	}
	if (retval == 0) {
		union sigval value;
		value.sival_int = 1000;
		sigqueue(par_id, 40, value);
		
		retval = close(in_fd);
		if (retval == -1) {
			printf("Failed to close input file\n");
			exit(EXIT_FALURE);
		}
		return;
	}

	union sigval value;
	value.sival_int = c;
	sigqueue(par_id, 40, value);
}

void sig40_sigaction(int code, siginfo_t *info_ptr, void *ptr) {	//data signal received
	if (info_ptr->si_value.sival_int == 1000) {
		int retval = close(out_fd);
		if (retval == -1) {
			printf("Unable to close output file\n");
			exit(EXIT_FALURE);
		}
		exit(EXIT_SUCCESS);
	}

	char c = info_ptr->si_value.sival_int;
	int retval = write(out_fd, (void *) &c, 1);
	if (retval != 1) {
		printf("Error occured while writing to file\n");
		exit(EXIT_FALURE);
	}
	union sigval value;
	sigqueue(chd_id, 41, value);
}

void sig41_sigaction(int code, siginfo_t *info_ptr, void *ptr) {	//request signal received
	char c;
	int retval = read(in_fd, (void *) &c, 1);
	if (retval == 0) {
		union sigval value;
		value.sival_int = 1000;
		sigqueue(par_id, 40, value);

		retval = close(in_fd);
		if (retval == -1) {
			printf("Failed to close input file\n");
			exit(EXIT_FALURE);
		}
		return;
	}
	if (retval == -1) {
		printf("Unable to read from input file\n");
		exit(EXIT_FALURE);
	}
	union sigval value;
	value.sival_int = c;
	sigqueue(par_id, 40, value);
}

int main (int argc, char *argv[]) {	//1st - input file, 2nd - output file
	if (argc < 3) {
		printf("Not enough arguments\n");
		return EXIT_FALURE;
	}
	in_pathname = argv[1];
	out_pathname = argv[2];

	struct sigaction new_act;
        new_act.sa_handler = sigterm_handler;
	new_act.sa_flags = 0;
	int retval = sigaction(SIGTERM, &new_act, NULL);
	if (retval == -1) {
		printf("Unable to change TERM signal behaviour\n");
		return EXIT_FALURE;
	}

	new_act.sa_handler = sig_chld_handler;
	retval = sigaction(SIGCHLD, &new_act, NULL);
	if (retval == -1) {
		printf("Unable to change CHLD signal behaviour\n");
		return EXIT_FALURE;
	}

	

	new_act.sa_handler = usr1_handler;
	new_act.sa_flags = 0;
	retval = sigaction(SIGUSR1, &new_act, NULL);
	if (retval == -1) {
		printf("Unable to set handler fo USR1 signal\n");
		return EXIT_FALURE;
	}

	
	new_act.sa_sigaction = sig40_sigaction;
	new_act.sa_flags = SA_SIGINFO;
	retval = sigaction(40, &new_act, NULL);
	if (retval == -1) {
		printf("Unable to change signal 40 sigaction\n");
		return EXIT_FALURE;
	}

	new_act.sa_sigaction = sig41_sigaction;
	new_act.sa_flags = SA_SIGINFO;
	retval = sigaction(41, &new_act, NULL);
	if (retval == -1) {
		printf("Unable to change signal 41 sigaction\n");
		return EXIT_FALURE;
	}



	par_id = getpid();
	chd_id = fork();
	if (chd_id == -1) {
		printf("Unsucceed to execute\n");
		return EXIT_FALURE;
	}

	if (chd_id == 0) {
		//child code
		while (1) {
			pause();		
		}
	} else {
		//parent code
		out_fd = creat(argv[2], 0777);
		if (out_fd == -1) {
			printf("Failed to open %s\n", argv[2]);
			return EXIT_FALURE;
		}
	
		retval = kill(chd_id, SIGUSR1);
	       	if (retval == -1) {
			printf("Failed to start conection\n");
			return EXIT_FALURE;
		}

		while (1) {
			pause();
		}	
	}
	
	return EXIT_SUCCESS;
}
