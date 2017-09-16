#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <time.h>
#include <string.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
class Thread
{
    private:
        pthread_t pid;
	    std::string host_;
        int batch_num_;
        std::vector<std::string> urls_;
    private:
        static void * start_thread(void *arg); //静态成员函数
    public:
		Thread() { }
        void set(std::string &host, std::vector<std::string> &urls, int batch_num = 1000)
        {
            host_ = host;
            urls_ = urls;
            batch_num_ = batch_num; 
	    }
        pthread_t getpid()
        {
    	   return pid;
        }	
        int start();
        void run();
};

int Thread::start()
{
    if(pthread_create(&pid,NULL,start_thread,(void *)this) != 0)
    {      
        return -1;
    }      
    return 0;
}
void* Thread::start_thread(void *arg)
{
    Thread *ptr = (Thread *)arg;
    ptr->run();
}
void Thread::run()
{
	//std::cout<<"Helloworld"<<std::endl;
// connect to ds
	
// execute batch delete
// exit while finish
}

void log(char *str)
{
	char log_time[64];
	time_t t = time(0);
	strftime( log_time, sizeof(log_time), "%Y-%m-%d %H:%M:%S",localtime(&t));
	std::ofstream fout("log",std::ios::app);
	if( fout.fail() )
	{
		std::cout<<"open log file error"<<std::endl;
		exit(0);
	}
	fout<<"[ "<<log_time<<" ] "<<str<<std::endl;
	fout.close();
}

int main(int argc, char *argv[])
{
	std::string host="127.0.0.1";
	int sockfd, port;
	struct sockaddr_in servaddr;
	/*sockfd = socket( AF_INET, SOCK_STREAM, 0);
	if( sockfd <0)
	{
		log("socket error");
		exit(0);
	}
	memset( &servaddr,0,sizeof(&servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons( port );
	if( inet_pton( AF_INET,host, &servaddr_sin.addr )<=0 )
	{
		log("error");
		exit(0);
	}
	if( connect( sockfd, (struct sockaddr*)&servaddr,sizeof(servaddr)) <0 )
	{
		log("connect error");
		exit(0);
	}
	close(sockfd);	*/

// host, ip, thread_num, area, batch_count, url_path
	int thread_num = 20, batch_count = 4;
	char  url_path[]="urls";
	Thread threads[thread_num];
	std::vector<std::string> tmp_urls;
	char tmp_file_line[2048];
	int read_file_cnt = 0, thread_cnt = 0;
	
	std::ifstream fin( url_path, std::ios::in);
	if( fin.fail() )
	{
		log("open url_path file error");
		exit(0);
	}
	while( fin.getline( tmp_file_line, 2048) != NULL )
	{
		read_file_cnt++;
		tmp_urls.push_back( tmp_file_line );
		if( read_file_cnt == batch_count )
		{
			threads[thread_cnt].set(host, tmp_urls, batch_count);
			if( threads[thread_cnt].start() == -1 )
			{
				log("create thread error");
				exit(0);
			}
			thread_cnt++;
			read_file_cnt = 0;
			tmp_urls.clear();
		}
	}
	if( tmp_urls.size() >0 )
	{
		threads[thread_cnt].set( host, tmp_urls, batch_count );
		if( threads[thread_cnt].start() == -1)
		{
			log("create thread error");
			exit(0);
		}
		thread_cnt++;
	}
	fin.close();
	for( int i=0 ; i<thread_cnt; i++)
	{
		pthread_t pid = threads[i].getpid();
		pthread_join(pid,NULL);
	}
	return 0;
}
