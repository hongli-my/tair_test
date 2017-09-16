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
	tair::tair_client_api client_helper;
	char *server_addr;
	//char *slave_server_addr
	char group_name[10];

	TfsClient tfs_client;
	string nsip;
public:
	void set_tair(char *server_addr_, char * group_name_){
		server_addr = server_addr_;
		group_name = group_name_;
		data = NULL;
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
