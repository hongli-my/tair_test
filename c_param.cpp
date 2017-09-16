#include<stdio.h>
#include<string>
#include<stdlib.h>
using namespace std;

int main(int argc, char *argv[])
{
	string str1;
	string str2;
	int i;
	while( (i = getopt(argc,argv,"s:c:h"))!=EOF)
	{
		switch(i)
		{
			case 'h':
				fprintf(stdout,"Usage......\n");
				break;
			case 'c':{
				str1 = optarg;
				fprintf(stdout,"%s\n",str1.c_str());
			}
			break;
			case 's':{
				str2 = optarg;
				fprintf(stdout,"-s ------>%s\n",str2.c_str());
			}
			break;
			default:
				fprintf(stderr,"----ERROR------\n");
		}
	}
	return 0;
}
