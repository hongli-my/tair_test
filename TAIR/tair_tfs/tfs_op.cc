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
	if( tfs_client.tfs_open(tfs_name, prefix, 1) !=TFS_SUCCESS ){
		fprintf(stdout,"open tfs_client fail: %s\n",tfs_client.get_error_message());
		exit(0);
	}
	FileInfo file_info;
	if( tfs_client.tfs_stat(&file_info) == TFS_ERROR){
		tfs_client.tfs_close();
		fprintf(stdout,"fstat tfs_client fail: %s\n",tfs_client.get_error_message());
		exit(0);
	}
	char data[1048576];
	//int total_size = 0;
	//int crc = 0;
	for(;;){
		int read_len = tfs_client.tfs_read_v2(data,1048576,&file_info);
		if(read_len < 0){
			fprintf(stdout,"read tfs_client fail: %s\n",tfs_client.get_error_message());
			tfs_client.tfs_close();
			exit(0);
		}
		if(read_len == 0){
			break;
		}
		fprintf(stdout,"read data: %s\n",data);
		//crc = Func::crc(crc, data, read_len);
		//total_size += read_len;
		if( tfs_client.is_eof()){
			break;
		}
	}
	tfs_client.tfs_close();
	//if (crc != file_info.crc_ || total_size != file_info.size_){
	//	fprintf(stderr, "%s, crc error: %u <> %u, size: %u <> %u\n", tbsys::CNetUtil::addrToString(
	//		tfs_client.get_last_elect_ds_id()).c_str(), crc, file_info.crc_, total_size, file_info.size_);
	//	return TFS_ERROR;
	//}
	return 0;
}
