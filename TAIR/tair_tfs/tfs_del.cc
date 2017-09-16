#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <vector>
#include <string>
#include <map>

#include "tfs_client_api.h"
using namespace std;
using namespace tfs::client;
using namespace tfs::common;
//using namespace tfs::message;

int main()
{
	TfsClient tfs_client;
	string nsip = "10.20.16.117:600";
	int ret = tfs_client.initialize(nsip);
	if(ret != TFS_SUCCESS){
		fprintf(stdout,"tfs initialize error\n");
		exit(0);
	}
	char tfs_name[30];
	strcpy(tfs_name, "T1ftETByJT1RCvBVdK");
	char * prefix = NULL;
	prefix = tfs_name + 18;
	
	ret = tfs_client.unlink(tfs_name,prefix);
	if(ret != EXIT_SUCCESS)
		fprintf(stderr,"unlink file error:%s\n",tfs_name);
	tfs_client.tfs_close();
	return 0;
}
