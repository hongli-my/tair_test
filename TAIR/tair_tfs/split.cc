#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <time.h>
#include <fcntl.h> 
using namespace std;

#define READ_BUF 1024*1024*3

void spilt_files(const char *source_file, const char *dest_file,char *datetime)
{
	char *file_name = new char[1024];
	int n = 100, num = 0;
	sprintf(file_name,"%s%s%d",dest_file,datetime,n);
	printf("%s\n",file_name);
	fprintf(stderr,"##############file op###############\n");
	int r_fd = -1, w_fd = -1;
	int write_byte, read_byte;
	if( (r_fd = open(source_file,O_RDONLY)) == -1){
		fprintf(stderr,"open %s error:%s\n",source_file,strerror(errno));
		delete file_name;
		return;
	}
	if( (w_fd = open(file_name,O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) == -1){
		fprintf(stderr, "open %s error:%s\n", file_name,strerror(errno));
		close(r_fd);
		delete file_name;
		return ;
	}
	char buffer[READ_BUF];
	char *ptr;
	while( read_byte = ::read(r_fd,buffer,READ_BUF)){
		if(read_byte == -1 && errno != EINTR){
			fprintf(stderr,"read op serror:%s\n",strerror(errno));
			break;
		}
		else if(read_byte > 0){
			if(num == 3){
				num = 0;
				if(w_fd != -1) close(w_fd);
				w_fd = -1;
				n++;
				sprintf(file_name,"%s%s%d",dest_file,datetime,n);
				if( (w_fd = open(file_name,O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) == -1){
					fprintf(stderr, "open %s error:%s\n", file_name,strerror(errno));
					break;
				}
			}
			ptr = buffer;
			while(write_byte = ::write(w_fd,ptr,read_byte)){
				if(write_byte == -1 && errno != EINTR){
					fprintf(stderr, "write op error:%s\n",strerror(errno));
					break;
				}
				else if(write_byte == read_byte) break;
				else if(write_byte > 0){
					ptr += write_byte;
					read_byte -= write_byte;
				}
			}
			if(write_byte == -1 ) break;
		}
		num++;
	}
	if(r_fd != -1) close(r_fd);
	if(w_fd != -1) close(w_fd);
	delete file_name;
	
}

int main(int argc, char **argv)
{
	if(argc !=2 ){
		fprintf(stderr,"Usage: %s disk|ssd\n",argv[0]);
		exit(0);
	}
	char source_file[1024];
	char dest_file[1024];
	time_t now;
	struct tm *tm_now;
	char datetime[20];
	time(&now);
	tm_now = localtime(&now);
	strftime(datetime,200,"%Y%m%d",tm_now);
	printf("%s\n",datetime);
	//################################################################
	
	strcpy(source_file,"/data/proclog/log/storage/evicted/data/");
	strcat(source_file,argv[1]);
	strcat(source_file,"/");
	strcat(source_file,datetime);
	strcat(source_file,"/");
	
	strcpy(dest_file,"/data/proclog/log/storage/evicted/need_delete/");
	strcat(dest_file,argv[1]);
	strcat(dest_file,"/");
	printf("source_file --->  %s\n",source_file);
	printf("dest_file   --->  %s\n",dest_file);
	spilt_files(source_file, dest_file, datetime);
	
	return 0;
}
