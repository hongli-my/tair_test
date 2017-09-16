#ifndef __Thread_work_H__
#define __Thread_work_H__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <pthread.h>
#include "Opjob.h"
#include "my_queue.h"
#include "tair_client_api.hpp"

class TestWork
{
public:
	TestWork():_queue(100){}
	~TestWork();
	void start();
	void run();
	bool offerJob(OpJob* job);
	void init(const char* server_addr, const char* group_name, const int area);
	void doWork(OpJob* do_job);
	static void* thread_func(void * argv);
	signed long long getTime();
	static signed long long total_time;
private:
	pthread_t _work_thread;
	char* _server_addr;
	char* _group_name;
	int _area;
	int _can_over;
	MY_Queue<OpJob*> _queue;
	tair::tair_client_api client_helper;
};
#endif