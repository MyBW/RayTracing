#pragma once
#include <Windows.h>
#include <vector>
class MutexLock;
class Mutex
{
public:
	static Mutex* Create()
	{
		return new Mutex();
	}
	friend MutexLock;
private:
	Mutex()
	{
		InitializeCriticalSection(&CriticalSection);
	}
	~Mutex()
	{
		DeleteCriticalSection(&CriticalSection);
	}
	CRITICAL_SECTION CriticalSection;
};
class MutexLock
{
public:
	MutexLock(Mutex &InM) :M(InM)
	{
		EnterCriticalSection(&M.CriticalSection);
	}
	~MutexLock()
	{
		LeaveCriticalSection(&M.CriticalSection);
	}
private:
	Mutex &M;
};

class Semaphore
{
public:
	Semaphore()
	{
		handle = CreateSemaphore(NULL, 0, 65535, NULL);
	}
	~Semaphore()
	{
		CloseHandle(handle);
	}
	void Wait()
	{
		WaitForSingleObject(handle, INFINITE);
	}
	void Post(int Count = 1)
	{
		ReleaseSemaphore(handle, Count, NULL);
	}
private:
	HANDLE handle;
};

class  ConditionVariable
{
public:
	ConditionVariable()
	{
		WaitNum = 0;
		SignalEvent = CreateEvent(NULL, false, false, NULL);
		InitializeCriticalSection(&ConditionCS);
		InitializeCriticalSection(&WaitNumCS);
	}
	void Lock()
	{
		EnterCriticalSection(&ConditionCS);
	}
	void Unlock()
	{
		LeaveCriticalSection(&ConditionCS);
	}
	void Signal()
	{
		EnterCriticalSection(&WaitNumCS);
		bool IsSendSignal = WaitNum > 0;
		LeaveCriticalSection(&WaitNumCS);

		if (IsSendSignal)
			SetEvent(SignalEvent);
	}
	void Wait()
	{
		EnterCriticalSection(&WaitNumCS);
		WaitNum++;
		LeaveCriticalSection(&WaitNumCS);

		LeaveCriticalSection(&ConditionCS);

		WaitForSingleObject(SignalEvent, INFINITE);

		EnterCriticalSection(&WaitNumCS);
		WaitNum--;
		LeaveCriticalSection(&WaitNumCS);

		EnterCriticalSection(&ConditionCS);
	}

private:
	CRITICAL_SECTION ConditionCS;
	CRITICAL_SECTION WaitNumCS;
	HANDLE SignalEvent;
	int WaitNum;
};


class Task
{
public:
	virtual void Run() = 0;
};

int NumSystemCores();
void TaskInit();
void EnqueueTasks(std::vector<Task*> InTaskList);
void WaitTaskListFinish();

void CleanupTaskList();

DWORD WINAPI ThreadFunc(LPVOID lParam);

