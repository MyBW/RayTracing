#include "Parallel.h"

std::vector<Task*> TaskList;
int AllTaskNum = 0;
HANDLE *ThreadHandles = NULL;
Mutex *TaskListMutex;
Semaphore *TaskListSyn = NULL;
ConditionVariable *TaskListRuningCondition = NULL;
void EnqueueTasks(std::vector<Task*> InTaskList)
{
	if (ThreadHandles == NULL)
	{
		TaskInit();
	}
	for (auto T : InTaskList)
	{
		TaskList.push_back(T);
	}
	AllTaskNum += InTaskList.size();
	TaskListSyn->Post(InTaskList.size());
	InTaskList.clear();
}
void WaitTaskListFinish()
{
	TaskListRuningCondition->Wait();
}
void CleanupTaskList()
{
	if (!TaskListRuningCondition || !TaskListMutex)
	{
		return;
	}
	static const int nThreads = NumSystemCores();
	if (TaskListSyn != NULL)
	{
		TaskListSyn->Post(nThreads);
	}
	if (ThreadHandles)
	{
		WaitForMultipleObjects(nThreads, ThreadHandles, true, INFINITE);
		for (int i = 0; i < nThreads; i++)
		{
			CloseHandle(ThreadHandles[i]);
		}
		delete[] ThreadHandles;
		ThreadHandles = NULL;
	}
}
DWORD WINAPI ThreadFunc(LPVOID lParam)
{
	TaskListSyn->Wait();
	while (true)
	{
		Task * CurrentTask = NULL;
		{
			MutexLock TMutexLock(*TaskListMutex);
			if (TaskList.size() == 0) break;
			CurrentTask = TaskList.back();
			TaskList.pop_back();
		}
		CurrentTask->Run();
		delete CurrentTask;
		TaskListRuningCondition->Lock();
		AllTaskNum--;
		if (AllTaskNum == 0)
		{
			TaskListRuningCondition->Signal();
		}
		TaskListRuningCondition->Unlock();
	}
	return 0;
}

int NumSystemCores()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}
void TaskInit()
{
	static const int nThreadNum = NumSystemCores();
	TaskListSyn = new Semaphore();
	TaskListRuningCondition = new ConditionVariable();
	TaskListMutex = Mutex::Create();
	ThreadHandles = new HANDLE[nThreadNum];
	for (int i = 0; i < nThreadNum; i++)
	{
		ThreadHandles[i] = CreateThread(NULL, 0, ThreadFunc, reinterpret_cast<void *>(i), 0, NULL);
	}
}