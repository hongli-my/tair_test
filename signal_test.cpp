/*
 * signal test
 * 进程表的表项中有一个软中断信号域,该域中每一位对应一个信号
 * 当有信号发送给进程时,对应位置位(进程对不同的信号，可以同时保存)
 */
#include<stdio.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
int log_level = 2;
void sign_Handler( int SignNum )
{
	switch( SignNum )
	{
		case 42:
			log_level++; break;
		case 41:
			log_level--; break;
	}
	printf("capture singal number:%d\n",SignNum);
}
int main()
{
	signal( 42, sign_Handler );
	signal( 41, sign_Handler );
	printf("main start pid = %d\n",getpid());
	while(1)
	{
		sleep(3);
		if( log_level > 2 )
			printf("log_level is :%d\n",log_level);
		else if( log_level < 2)			
			printf("log_level is :%d\n",log_level);
		printf("this is in while\n");
	}
	return 0;
}
