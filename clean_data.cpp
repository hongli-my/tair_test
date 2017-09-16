#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>

class ReadFile
{
private:
	std::vector<char *> _file_name;
	std::vector<char *> _TaskQueue;
public:
	void create();
	void Dis()
	{
		puts("-------------------------");
		for(int i=0; i<(int)_file_name.size(); i++)
			printf("%s\n",_file_name[i]);
	}
	ReadFile( std::vector<char *> &file_name )
	{
		_file_name = file_name;
	}
	~ReadFile() {}
	int AddTask();
	void Read_string();
	void DoTask();
};
int main(int argc ,char** argv)
{
	if( argc != 2 )
	{
		fprintf(stderr,"[%s:%d] invalid argument!",__FILE__, __LINE__);
		exit(1);
	}
	char dir[256];
	strcpy(dir,argv[1]);
	if( dir[strlen(dir)-1] != '/' )
		strcat(dir,"/");

	DIR *dp = NULL;
	struct dirent *dirp = NULL;
	if( ( dp = opendir(dir) ) ==NULL )
	{
		fprintf(stderr,"[%s:%d] opendir error! --> %s\n",__FILE__, __LINE__,strerror(errno));
		exit(1);
	}
	std::vector<char *>	file_name;
	char * tmp_str = (char *) malloc(4096);
	char *q = tmp_str;
	while( ( dirp = readdir(dp) ) != NULL )
	{
		if( strcmp(dirp->d_name,".")==0 || strcmp(dirp->d_name,"..")==0 )
			continue;

		strcpy(tmp_str,dir);
		strcat(tmp_str,dirp->d_name);
		struct stat st;
		stat(tmp_str,&st);
		if( S_ISREG(st.st_mode) )
			file_name.push_back( tmp_str );
		int len = strlen(tmp_str);
		char *p = tmp_str + len;
		//p = '\0';
		tmp_str = p + 1;
	}
	for(int i=0; i<(int)file_name.size(); i++)
	{
		printf("%s\n",file_name[i]);
	}
	ReadFile tt(file_name);
	tt.Dis();
	free(q);
	return 0;
}
