#include"my_test_head.h"
using namespace std;

int main(int argc, char* argv[])
{
	DIR	*dp;
	struct dirent *dirp;
	if( argc != 2)
	{
		fprintf(stderr,"[%s:%d] argument error!\n",__FILE__, __LINE__);	
		exit(1);
	}
	if( (dp = opendir(argv[1]) ) == NULL )
	{
		fprintf(stderr,"[%s:%d] opendir error! --> %s\n",__FILE__, __LINE__,strerror(errno));
		exit(1);
	}
	while( (dirp = readdir(dp) )!=NULL )
		printf("%s\n",dirp->d_name);
	closedir(dp);
	return 0;
}

