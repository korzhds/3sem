#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#define LAST_MESSAGE 255			//для прекращения сообщения
#define SIZE 4024 //размер текста в сообщении


int main(int argc, char* argv[]) {
	int msqid;
	char pathname[] = "f_sender.c";
	key_t key;
	int fd, length, i;
	struct mymsgbuf {
		long mtype;   //message type; long is essential
		char mtext[SIZE];
		int bytes;
	} mybuf;
	size_t readbytes;
	if ((key = ftok(pathname, 0)) < 0) {//генерация ключа
		printf("Can't generate key\n");
		exit(-1);
	}
	if ((msqid = msgget(key, 0666|IPC_CREAT)) < 0) { //пытаемся получить доступ по ключу к очереди сообщений или создать RW для всех
		printf("Can't get msqid\n");
		exit(-1);
	}
	char* filename = argv[1];
	if ((fd = open(filename, O_RDWR, 0666)) < 0) {//открываем файлик
		printf("Can't open file\n");
		exit(-1);
	}
	do {
		readbytes = read(fd, (void*)&(mybuf.mtext), SIZE);//считываем размерчик
		if (readbytes < 0) {
			printf("Read error\n");
			exit(-1);
		}
		if (readbytes == SIZE) {
			mybuf.mtype = 1;
			mybuf.bytes = readbytes;
		} else {
			mybuf.mtype = LAST_MESSAGE;
			mybuf.bytes = readbytes;
		}
		if ((i = msgsnd(msqid, (struct msgbuf *) &mybuf, SIZE + 5, 0)) < 0) {//помещение сообщения в очередь сообщений
			printf("Can't send message\n");
			msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);//удаляем
			exit(-1);
		}
	} while (readbytes == SIZE);
	if ((i = close(fd)) < 0) {
		printf("Can't close file\n");
		exit(-1);
	}
	return 0;
}

