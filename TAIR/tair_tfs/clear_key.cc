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

int main()
{
	TBSYS_LOGGER.setLogLevel("WARN");
	tair::tair_client_api client_helper;
	client_helper.set_timeout(5000);
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
	char *pkey = new char[1024];
	strcpy(pkey,"key1");
	tair::data_entry key(pkey, strlen(pkey), false);
	tair::data_entry *data = NULL;
	int ret = client_helper.get(1, key, data);
	if (ret != TAIR_RETURN_SUCCESS && ret != TAIR_RETURN_DATA_EXPIRED) {
		fprintf(stdout, "get failed: %d.%s.\n",ret,client_helper.get_error_msg(ret));
	} 
	else if (data != NULL) {
		fprintf(stdout, "KEY: %s, DATA_LEN: %d\n raw data: %s\n", pkey, data->get_size(), data->get_data());
		delete data;
	}
	delete pkey;
	return 0;
}
