#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <sys/fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "Opjob.h"
#include "Thread_work.h"
#include "tair_client_api.hpp"
using namespace std;

class TairTest
{
public:
	TairTest(const char* server_addr,const char* group_name, const int area, const char* keyfile, const int work_nums);
	~TairTest();
	void execute();
	void PostJob(OpJob* job);
	void gen_str(char *str,const int len);
	void do_work();
	void get_wfd();
	void get_rfd();
	static long long total_op;

private:
	int _work_nums;
	int w_fd;
	FILE* r_fd;
	long read_cur_pos;
	long del_cur_pos;
	char* _KeyFile;
	int _worker_selected;
	TestWork* _workers;
};
TairTest::TairTest(const char* server_addr,const char* group_name, const int area, const char* keyfile, const int work_nums)
{
	//int len_key_file = strlen(keyfile) + 1;
	_KeyFile = new char[10]; // use try catch to check new fail.
	strcpy(_KeyFile,"./keyfile");
	_work_nums = work_nums;
	_worker_selected = 0;
	w_fd = -1;
	r_fd = NULL;
	read_cur_pos = 0;
	del_cur_pos = 0;

	_workers = new TestWork[work_nums];
	for(int i=0; i< work_nums; i++)
	{
		_workers[i].init(server_addr, group_name, area);
	}
}
TairTest::~TairTest()
{
	if(_KeyFile != NULL)
		delete []_KeyFile;
	if(_workers != NULL)
		delete []_workers;
	if(w_fd != -1)
		close(w_fd);
	if(r_fd != NULL)
		fclose(r_fd);
}
void TairTest::get_wfd()
{
	if(w_fd == -1)
	{
		if( ( w_fd = open(_KeyFile,O_WRONLY|O_APPEND,S_IWUSR|S_IRUSR) ) == -1 )
		{
			fprintf(stderr, "open file error\n");
		}
	}
}
void TairTest::get_rfd()
{
	if(r_fd == NULL)
	{
		if( (r_fd = fopen(_KeyFile,"r"))==NULL )
		{
			fprintf(stderr,"fopen file error\n");
		}
	}
}
void TairTest::gen_str(char *str,const int len)
{
	char *p = str;
	for(int i=0;i<len;++i)
		(*p++)=rand()%94 + 33;  //  character between [!,~]
	*p='\0';
}
void TairTest::execute()
{
	for(int i=0; i<_work_nums; i++)
	{
		_workers[i].start();  // start thread
	}
	do_work();
	fprintf(stdout,"Tair test total operation:%lld\n",total_op);
}
void TairTest::PostJob(OpJob* job)
{
	int ok = false;
	while(true)
	{
		ok = _workers[_worker_selected].offerJob(job);
		if(ok == true)
			break;
		_worker_selected = (_worker_selected + 1)%_work_nums;
	}
	_worker_selected = (_worker_selected + 1)%_work_nums;
}
void TairTest::do_work()
{
	for(int test_case =0 ; test_case<3; test_case++)
	{
		int flag = rand()%3;  // 0 put, 1 get, 2 delete
		int nums = rand()%90 + 10;
		total_op = total_op + nums;
		//flag = 1;
		//nums = 5;
		if(flag == 0)
		{
			get_wfd();
			int rand_key_len = rand()%10 + 5;
			int rand_val_len = rand()%20 + 5;
			char key[rand_key_len];
			char val[rand_val_len];
			OpJob *job = new OpJob(nums,0);
			for(int i=0; i<nums; i++)
			{
				gen_str(key, rand_key_len);
				gen_str(val, rand_val_len);
				if( write(w_fd, key, strlen(key))== -1 )
				{
					fprintf(stderr,"write error\n");
					exit(1);
				}
				write(w_fd,"\n",1);
				//fprintf(stderr, "key->%s\n",key);
				//fprintf(stderr, "val->%s\n",val);
				job->add(key,rand_key_len,val,rand_val_len);
			}
			PostJob(job);
			close(w_fd);
			w_fd = -1;
		}
		else
		{
			int cur_pos = read_cur_pos;
			if(flag == 2)
			{
				cur_pos = del_cur_pos;
			}
			get_rfd();
			fseek(r_fd, cur_pos, SEEK_SET);
			char key[1024+10];
			OpJob *job = new OpJob(nums,flag); // operation type: get or delete
			int reopen_file_num = 0;
			for(int i=0; i<nums; i++)
			{
				if( fgets(key,1024,r_fd) == NULL)
				{
					if(feof(r_fd))
					{
						fprintf(stdout,"reache end of file\n");
						reopen_file_num++;
						if(reopen_file_num > 4)
						{
							fprintf(stderr, "keyfile maybe empty\n");
							total_op = total_op - nums;
							delete job;
							goto last;
						}
						fseek(r_fd, 0, SEEK_SET);
						i--;
						continue;
					}
					fprintf(stderr,"fgets error\n");
					exit(1);
				}
				int key_size = strlen(key);
				key[key_size-1] = '\0';
				job->add(key,key_size);
			}
			PostJob(job);
			if(flag == 1)
				read_cur_pos = ftell(r_fd);
			else if(flag == 2)
				del_cur_pos = ftell(r_fd);
			last:
			fclose(r_fd);
			r_fd = NULL;
		}
	}
}

signed long long TestWork::total_time = 0;
long long TairTest::total_op = 0;
//-----------------------------------------------
int main(int argc, char *argv[])
{
	char *group_name = NULL;   //diskcache
	char *server_addr = NULL;  // ip:port
	int  area = 1;         // default 1
	int work_nums = 0;         // thread nums
	int i;
	while( (i = getopt(argc,argv,"c:g:n:h:"))!=EOF)
	{
		switch(i)
		{
			case 'c':
				server_addr = optarg;
				break;
			case 'g':
				group_name = optarg;
				break;
			case 'n':
				work_nums = atoi(optarg);
				break;
			default:
				fprintf(stdout,"Usage......%s -c ip:port -g group_name -n work_nums\n",argv[0]);
				exit(0);
		}
	}
	if( server_addr == NULL || group_name == NULL)
	{
		fprintf(stderr,"argument error!\n");
		exit(0);
	}
	fprintf(stdout, "=============================\n");
	fprintf(stdout,"server_addr : %s\n",server_addr);
	fprintf(stdout,"group_name  : %s\n",group_name);
	fprintf(stdout,"area        : %d\n",area);
	fprintf(stdout,"work_nums   : %d\n",work_nums);
	fprintf(stdout,"=============================\n");
	TBSYS_LOGGER.setLogLevel("ERROR");
	TBSYS_LOGGER.setFileName("./tair_test_log");
	srand(time(NULL));
	TairTest tool(server_addr,group_name,area,NULL,work_nums);
	tool.execute();
	return 0;
}