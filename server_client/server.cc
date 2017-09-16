#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>


#define MAXLINE 4096

int main(int argc, char** argv)
{
    int    listenfd, connfd;
    struct sockaddr_in   servaddr, tmp_addr;
    char    buff[4096];
    int     n;

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
    	printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
    	exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
    	printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
    	exit(0);
    }

    if( listen(listenfd, 10) == -1){
    	printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
    	exit(0);
    }

    printf("======waiting for client's request======\n");
	char tmp_ip[20];
    while(1){
    	if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
        	printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
        	continue;
    	}
		socklen_t  tmp_len = sizeof(tmp_addr);
		if( getpeername(connfd,(struct sockaddr*)&tmp_addr,&tmp_len) ==0){
			inet_ntop( AF_INET, &tmp_addr.sin_addr, tmp_ip, sizeof(tmp_ip) );
			printf("%s:%d\n",tmp_ip,ntohs(tmp_addr.sin_port));
		}
    	n = recv(connfd, buff, MAXLINE, 0);
    	buff[n] = '\0';
    	printf("recv msg from client: %s\n", buff);
    	close(connfd);
    }

    close(listenfd);
	return 0;
}
