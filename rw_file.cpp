#include"my_test_head.h"

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("Usage ./a.out pathname\n");
		exit(1);
	}
	char *pathname = argv[1];
	int fd;
	if( ( fd = open(pathname,O_WRONLY|O_APPEND,S_IWUSR|S_IRUSR) ) == -1 )
	{
		/*flags:O_RDONLY |O_WRONLY 
 		* O_TRUNC 以可写方式打开时清空文件内容，
 		*O_APPEND  与上面相反 
 		* mode : S_IRUSR: 读权限，文件属主
		*S_IWUSR:写权限，文件属主
		*S_ IXUSR:执行权限，文件属主
		*S_IRGRP:读权限 文件所属组
		*S_IWGRP:写权限，文件所属组
  		*/
		fprintf(stderr,"[%s:%d] open error --> %d\n",__FILE__,__LINE__,strerror(errno));
		exit(1);
	}
	char *buf = new char(100);
	scanf("%s",buf);
	if( write(fd, buf, strlen(buf))== -1 )  // can not use sizeif(buf),
		fprintf(stderr,"[%s:%d] open error --> %d\n",__FILE__,__LINE__,strerror(errno));
	else printf("Yes\n");
	//while( read(fd,buf,sizeof(buf)) > 0 )
	//	printf("%s\n",buf);
	delete buf;
	close(fd);
	return 0;
}
