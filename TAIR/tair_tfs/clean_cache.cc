#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <vector>
#include <string>
#include <map>
#include "tair_client_api.hpp"
#include "tfs_client_api.h"
using namespace std;
using namespace tfs::client;
using namespace tfs::common;

class clean_op{
	char *server_addr;
	//char *slave_server_addr
	char *group_name;
	string nsip;
public:
	tair::tair_client_api client_helper;
	TfsClient tfs_client;
	void set_tair(char *server_addr_, char * group_name_){
		server_addr = server_addr_;
		group_name = group_name_;
	}
	void set_tfs(char *ns_ip_){
		nsip = ns_ip_;
	}
	bool op_init(){
		//client_helper.set_timeout(5000);
		bool done = client_helper.startup(server_addr, "NULL",group_name);
		int ret = tfs_client.initialize(nsip);
		if(ret != TFS_SUCCESS || done == false)
			return false;
	}
	void stop(){
		client_helper.close();
		tfs_client.tfs_close();
	}
};

int main(int argc, char **argv)
{
	if(argc !=2 ){
		fprintf(stderr,"Usage: %s file_name\n",argv[0]);
		exit(0);
	}
	TBSYS_LOGGER.setLogLevel("ERROR");
	TBSYS_LOGGER.setFileName("/data/proclog/log/storage/evicted/key_log");
	FILE* stream;
	stream = fopen(argv[1],"r");
	char buf[1024];
	clean_op clean;
	char server_ip[10];
	char ns_ip[10];
	char groupname[10];
	strcpy(ns_ip,"10.20.62.68:600");
	strcpy(server_ip,"10.20.62.69:700");
	strcpy(groupname,"diskcache");
	clean.set_tair(server_ip,groupname);
	clean.set_tfs(ns_ip);
	clean.op_init();
	while(fgets(buf,sizeof(buf),stream) != NULL){
		buf[strlen(buf)-1] = '\0';
		//printf("read buf: %s\n",buf);
		log_error("read tair key : %s",buf);
		tair::data_entry key(buf, strlen(buf), false);
		tair::data_entry *data = NULL;
		int ret = clean.client_helper.get(1, key, data);
		if (ret != TAIR_RETURN_SUCCESS && ret != TAIR_RETURN_DATA_EXPIRED) {
			//fprintf(stdout, "get failed: %d.%s.\n",ret,clean.client_helper.get_error_msg(ret));
			log_error("get tfs_kye from tair fail : %s",clean.client_helper.get_error_msg(ret));
			if(data != NULL) delete data;
			continue;
		}
		log_error("get data from tair : %s",data->get_data());
		char tfs_name[1024];
		strcpy(tfs_name,data->get_data());
		char *prefix = tfs_name + 18;
		log_error("tfs_name : %s",tfs_name);
		ret = clean.tfs_client.unlink(tfs_name, prefix);
		log_error("result from tfs rm : %d",ret);
		if(ret != EXIT_SUCCESS){
			if(data != NULL) delete data;
			continue;
		}
		//printf("%d\n",ret);
		ret = clean.client_helper.remove(1,key);

		log_error("result from tair rm : %d",ret);
		//printf("%d\n",ret);
		if(data != NULL) delete data;
	}
	fclose(stream);
	clean.stop();
	return 0;
}
