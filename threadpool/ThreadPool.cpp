#include "ThreadPool.h"
#include<iostream>
#include<string.h>
#include<string>
using namespace std;
ThreadPool::ThreadPool(int min, int max)
{
	//实例化任务队列
	taskQ = new TaskQueue;
	do
	{
		threadIDs = new pthread_t[max];
		if (threadIDs == nullptr) 
		{
			cout << "new threadIDs fail..." << endl;
			break;
		}
		memset(threadIDs, 0, sizeof(pthread_t) * max);
		minNum = min;
		maxNum = max;
		busyNum = 0;
		aliveNum = min;
		exitNum = 0;
		if (pthread_mutex_init(&mutexPool, NULL) != 0 || pthread_cond_init(&notEmpty, NULL) != 0)
		{
			cout << "mutex or condition init fail...\n";
			break;
		}
		shutdown = false;
		pthread_create(&managerID, NULL,manager,this );
		for (int i = 0;i<min;)
		{

		}
	} while (0);
}

ThreadPool::~ThreadPool()
{
}

void ThreadPool::addTask(Task task)
{
}

int ThreadPool::getBusyNumber()
{
	return 0;
}

int ThreadPool::getAliveNumber()
{
	return 0;
}

void* ThreadPool::worker(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	return nullptr;
}

void* ThreadPool::manager(void* arg)
{
	return nullptr;
}

void ThreadPool::threadExit()
{
}
