#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
using namespace std;

/* remove()会删除参数pathname指定的文件。
 * 如果参数pathname为一文件，则调用unlink()处理，
 * 若参数pathname为一目录，则调用rmdir()来处理。请参考unlink()与rmdir()。
 * 返回值 成功则返回0，失败则返回-1
 */

int main()
{
	char *path = ""
	if( remove( path) != 0 )
	{
		fprintf(stderr,"[%s:%d] del file error!\n",__FILE__, __LINE__);
	}
	else
	{
		printf("Yes1");
	}
	if( access( path, F_OK ) !=0 )  ////OK（可读）、W_OK（可写）、X_OK（可执行）、F_OK（文件存在）
	{
		fprintf(stderr,"[%s:%d] not exists!\n",__FILE__, __LINE__);
	}
	else
	{	
		printf("exists!");
	}
}
