#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define N 100000000 //число точек
//#define n 1

double a, b, min, max;

int *array; //Указатель на разделяемую память для синхронизации
int n; //число потоков

typedef struct {
	int num;
} input_t;


void * generate(void * dummy); //предобъявим


int main() {
	a = 0;
	b = 5;
	min = -1000;
	max = 1000; //чтобы наверняка
	
	printf("Enter the number of threads\n");
	scanf("%d", &n);
	pthread_t *thread = (pthread_t *) malloc (n * sizeof(pthread_t));
	srand(time(NULL));
	input_t *t = (input_t *) malloc (n * sizeof(input_t));

	int shmid; //IPC дескриптор на разделяемую память
	char pathname[] = "Calculator.c";//Для генерации ключа
	
	key_t key;//IPC ключ
	long long i;
	
	/*Генерируем IPC ключ из им.файла и номера экземпляра области разделяемой памяти 0*/
	if ((key = ftok(pathname,0)) < 0) {
		printf("Can't generate key\n");
		exit(-1);
		
		/*Пытаемся экслюзивно создать разделяемую память для key (существует - return'ится <0*). Опр размер
		   памяти как размер массива из n целых перемменных*/
	}
	if ((shmid = shmget(key, n * sizeof(int), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
		if (errno != EEXIST) {
			printf("Can't create shared memory\n");  //Ошибка: НЕ из-за того,что сегмент разд памяти уже сущ.
			exit(-1);              
		} else {
		if ((shmid = shmget(key, n * sizeof(int), 0)) < 0) {
			printf("Can't find shared memory\n");        //Ошибка:  из-за того,что сегмент разд памяти уже сущ.
			exit(-1);									//пытаемся получить IPC shmemory
			}
		}
	}
	/*Пытаемся отобразить разд память в адресное про-во текущ процесса*/
	array = (int*)shmat(shmid, NULL, 0);
	if (array == (int*) (-1)) {
		printf("Can't attach shared memory\n");
		exit(-1);
	}

	for (i = 0; i < n; i++) {
		array[i] = 0;
	}


	//создаем нити, проверяем состояние
	for (i = 0; i < n; i++) {
		t[i].num = i;
		int st =pthread_create (&thread[i], (pthread_attr_t * )NULL, generate, (void*)&t[i]); 
		if (st != 0) {
			printf("Thread can't be created\n");
			exit(-1);
		}
	}
	
	/*блокируем работу нитей до завершения треда, после разблок в указатель
	  заносится адрес, кот. вернул заверш тред (либо из ассоц). Нас не волнует, что возвращает, поэтому в NULL*/
	for (i = 0; i < n; i++) {
		pthread_join(thread[i], (void **)NULL);
	}
	//проверяем  исключение области разд памяти из адр пр-ва текущ процесса
	if (shmdt(array) < 0) {
		printf("Can't detach shared memory\n");
		exit(-1);
	}
	free(thread);
	free(t);
	return 0;
	//array[n]=min;
	//array[n+1]=max;
}

double func(double q)
{
	return q*q; 
}


void * generate(void * dummy) {
	double x, y, yf;
	long long j, jmax;
	
	unsigned int seed1 = (unsigned int)time(NULL);
	unsigned int seed2 = (unsigned int)pthread_self();
	srand((unsigned)time(NULL));
	
	input_t * data = (input_t *)dummy;
	int i = data->num;
	if (i != 0) {
		jmax = N / n; //равномерность
	} else {
		jmax = (N / n) + (N % n);
	}
	int k = 0;
	for (j = 0; j < jmax ; j++) {
		
		x = (double)(rand_r(&seed1)) / RAND_MAX;
		x = (x * (b - a)) + a;
	/*	x=a+((double)rand()/RAND_MAX*(b-a));         */
		y = (double)(rand_r(&seed2))/ RAND_MAX;
	/*	y=(double)rand()/RAND_MAX;                    */
		y = (y * (max - min)) + min;
		yf=func(x);
		if ((y <= yf && y>=0)){
			k ++;
		}
		else
		if(y>=yf && y<=0){
			k--;
		}
	}
	array[i] = k;
	
	return 0;
}
 
