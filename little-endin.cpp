#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<time.h>
#include<errno.h>
#include<signal.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<arpa/inet.h>
/*
int main()
{
	union{
		short s;
		char c[ sizeof(short)];
	}un;
	un.s = 0x0102;
	printf("%d\n",un.s);
	if( sizeof(short)==2 )
	{
		if(un.c[0]== 1 &&un.c[1]==2 )
			printf("big-endin\n");
		else if( un.c[0]==2&&un.c[1]==1)
			printf("little-endin\n");
		else
			printf("unknown\n");
	}
	return 0;
}
*/
int main()
{
	int i=1297; //  0000 0101  0001 0001
	printf("i+1 == %p . %d, \n" , ((char*)&i), *(((char*)(&i))) );
	printf("i+1 == %p . %d, \n" , ((char*)&i)+1, *(((char*)(&i))+1) );
	printf("i+1 == %p . %d, \n" , ((char*)&i)+2, *(((char*)(&i))+2) );
	printf("i+1 == %p . %d, \n" , ((char*)&i)+3, *(((char*)(&i))+3) );
	if( *((char*)&i) == 5 )
		printf("big endin\n");
	else if( *((char*)&i) == 17)
		printf("little endin\n");
	else
		printf("unknown\n");
	i = ntohl(i);  //  reverse binary
	printf("i+1 == %p . %d, \n" , ((char*)&i), *(((char*)(&i))) );
	printf("i+1 == %p . %d, \n" , ((char*)&i)+1, *(((char*)(&i))+1) );
	printf("i+1 == %p . %d, \n" , ((char*)&i)+2, *(((char*)(&i))+2) );
	printf("i+1 == %p . %d, \n" , ((char*)&i)+3, *(((char*)(&i))+3) );
	return 0;
}
