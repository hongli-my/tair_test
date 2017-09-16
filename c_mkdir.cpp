#include<string.h>
#include<stdio.h>
#include<iostream>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
using namespace std;
#define DIR_MODE 0777 
#define BUFLEN 255

int mkdirs( const char *path)
{
	if( path == NULL )
	{
		fprintf(stderr,"[%s:%d] path is NULL pointer!\n",__FILE__, __LINE__);
		return -1;
	}
	char DirName[BUFLEN];
	strcpy(DirName,path);
	int i,len;
	if( (len=strlen(DirName))==0 )
	{
		fprintf(stderr,"[%s:%d] strlen(DirName) is 0\n",__FILE__, __LINE__);
		return -1;
	}
	if( DirName[len-1] != '/')
		strcat( DirName,"/");
	len = strlen(DirName);
	for( i=1; i<len; i++ )
	{
		if(DirName[i] == '/')
		{
			DirName[i]=0;
			if( access( DirName,F_OK) <0)
			{
				 if(mkdir(DirName,DIR_MODE) < 0)
				{
					fprintf(stderr,"[%s:%d] mkdir %s error!:%s\n",__FILE__,__LINE__,DirName,strerror(errno));
					return -1;
				}
			}
			DirName[i]='/';
		}
	}
	return 0;
}
int main()
{
	char p[20];
	//strcpy(p,"/usr/local/tmp");
	cin>>p;
	int res = mkdirs(p);
}
