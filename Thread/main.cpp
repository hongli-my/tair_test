#include "Thread.hpp"

class CMyTask: public CTask
{
public:
	CMyTask(){}
	
	inline int Run()
	{
		printf("%s\n", (char*)this->m_ptrData);
		sleep(3);
		return 0;
	}
};

int main()
{
	CMyTask taskObj;
	
	char szTmp[] = "this is the first thread running";
	taskObj.SetData((void*)szTmp);
	CThreadPool threadPool(8);
	
	for(int i = 0; i < 20; i++)
	{
		threadPool.AddTask(&taskObj);
	}
	
	while(1)
	{
		printf("there are still %d tasks need to handle\n", threadPool.getTaskSize());
		if (threadPool.getTaskSize() == 0)
		{
			if (threadPool.StopAll() == -1)
			{	
				printf("Now I will exit from main\n");
				exit(0);
			}
		}
		sleep(1);
	}
	
	return 0;
}
