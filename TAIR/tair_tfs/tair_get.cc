#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "tair_client_api.hpp"
using namespace std;

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stderr,"Usage:%d file_name\n",argv[0]);
		exit(0);
	}
	TBSYS_LOGGER.setLogLevel("WARN");
	TBSYS_LOGGER.setFileName("./tair_get_log");
	tair::tair_client_api client_helper;
	//client_helper.set_timeout(5000);
	//client_helper.set_force_service(true);
	const char *server_addr = "127.0.0.1:700";
	const char *slave_server_addr = NULL;
	char group_name[10];
	strcpy(group_name,"diskcache");
	bool done = client_helper.startup(server_addr, slave_server_addr,group_name);
	if(!done){
		fprintf(stdout,"connect error\n");
		exit(0);
	}
	fprintf(stdout,"connect success\n");
	char buf[1024];
	FILE *stream = fopen(argv[1],"r");
	while(fgets(buf,sizeof(buf),stream) != NULL){
		buf[strlen(buf)-1] = '\0';
		tair::data_entry key(buf, strlen(buf), false);
		tair::data_entry *data;
		int ret = client_helper.get(1, key, data);
		log_error("get: %s",client_helper.get_error_msg(ret));
		if(data != NULL) delete data;
	}
	fclose(stream);
	client_helper.close();
	return 0;
}
