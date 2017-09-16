#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

int num = 0;
pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
void *thread_func( void *arg)
{
	int i = (long long)arg;
	int ret;
	sleep(5-i);
	pthread_mutex_lock(&mylock);
	while( i!=num )
	{
		ret = pthread_cond_wait(&qready,&mylock);
	}
	printf("thread %d is running \n",i);
	num++;
	pthread_mutex_unlock(&mylock);	//unlock
	pthread_cond_broadcast(&qready); //signal all threads which wait condition
	return (void *)0;
}

int main()
{
	int i =0,err;
	pthread_t tid[4];
	void *ret;
	for( ;i<4; i++)
	{
		err = pthread_create(&tid[i],NULL,thread_func,(void *)i);
		if( err!=0 )
		{
			printf("pthread_create error\n");
			exit(-1);
		}
	}
	for( i=0; i<4; i++)
	{
		err = pthread_join( tid[i], &ret);
		if( err!=0 )
		{
			printf("can not join with thread %d:%s\n",i,strerror(err));
			exit(-1);
		}
	}
	return 0;
}
