#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <vector>
#include <sys/time.h>

#include "Thread_work.h"
#include "Opjob.h"
#include "my_queue.h"
#include "tair_client_api.hpp"

pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;
void TestWork::init(const char* server_addr, const char* group_name, const int area)
{
	_can_over = 0;
	int len_server_addr = strlen(server_addr) + 1;
	_server_addr = new char[len_server_addr];
	strcpy(_server_addr,server_addr);

	int len_group_name = strlen(group_name) + 1;
	_group_name = new char[len_group_name];
	strcpy(_group_name,group_name);
	_area = area;
}
void TestWork::start()
{
	bool done = client_helper.startup(_server_addr,NULL,_group_name);
	if(done == false)
	{
		fprintf(stderr,"can not connect to cs: %s grou_name : %s\n",_server_addr,_group_name);
		return;
	}
	int flag =  pthread_create(&_work_thread, NULL, thread_func, this);
	fprintf(stdout, "create thread ip =%u\n",(unsigned int)_work_thread);
	if(flag < 0)
	{
		fprintf(stderr,"file %s,line %d: %s\n",__FILE__,__LINE__,strerror(errno));
		return;
	}
}
TestWork::~TestWork()
{
	_can_over = 1;
	pthread_join(_work_thread,NULL);
	if(_server_addr != NULL)
	{
		delete []_server_addr;
	}
	if(_group_name != NULL)
	{
		delete []_group_name;
	}
	client_helper.close();
}
void* TestWork::thread_func(void *argv)
{
	TestWork* worker = (TestWork*)argv;
	worker->run();
	return NULL;
}
signed long long TestWork::getTime()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return (static_cast<signed long long>(t.tv_sec) * static_cast<signed long long>(1000000) + static_cast<signed long long>(t.tv_usec));
}
void TestWork::run()
{
	while(true)
	{
		if(_queue.isEmpty())
		{
			if(_can_over)
				break;
			fprintf(stderr,"thread %u queue is empty ,while usleep\n",(unsigned int)_work_thread);
			usleep(1000);  //micro_seconds
			continue;
		}
		OpJob* do_job = _queue.pop();
		
		//signed long long before = getTime();
		
		doWork(do_job);
		
		//signed long long after = getTime();
		//fprintf(stdout,"Tair test use time:%llu\n",after - before);
		//total_time = total_time + (after -before);
		//fprintf(stdout,"Tair test use time --->%llu\n",total_time);
		
		delete do_job;
	}
}
bool TestWork::offerJob(OpJob *job)
{
	if(_queue.isFull())
		return false;
	_queue.push(job);
	return true;
}
void TestWork::doWork(OpJob* do_job)
{
	int flag = do_job->_flag;
	int size = do_job->getSize();
	//std::vector<tair::data_entry*> keys_v;
	//std::vector<tair::data_entry*> vals_v;
	fprintf(stderr,"OpJob flag:%d\n",flag);
	if(flag == 0)
	{
		char** keys = do_job->getAllKey();
		char** vals = do_job->getAllVal();
		for(int i=0; i<size; i++)
		{
			//fprintf(stdout,"put key = %s\n",keys[i]);
			//fprintf(stdout,"val=%s\n",vals[i]);
			//fprintf(stderr,"thread %u\n",(unsigned int)_work_thread);
			tair::data_entry key(keys[i], strlen(keys[i]),false);
			tair::data_entry val(vals[i], false);
			int ret = client_helper.put(1,key,val,0,0);
			log_error("put: %s",client_helper.get_error_msg(ret));
		}
	}
	else
	{
		char ** keys = do_job->getAllKey();
		for(int i=0; i<size; i++)
		{
			//fprintf(stderr,"%s key = %s\n",(flag==1?"get":"delete"), keys[i]);
			tair::data_entry key(keys[i], strlen(keys[i]), false);
			if(flag == 1)
			{
				tair::data_entry *data = NULL;
				int ret = client_helper.get(1,key,data);
				log_error("get: %s",client_helper.get_error_msg(ret));
				if(data != NULL)
				{
					delete data;
					data = NULL;
				}
			}
			else
			{
				int ret = client_helper.remove(1,key);
				log_error("delete: %s",client_helper.get_error_msg(ret));
			}
		}
	}
}