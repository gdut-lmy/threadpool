#include "ThreadPool.h"

#include<iostream>
#include<cstring>
#include<string>
#include<unistd.h>
using namespace std;
template<typename T>
ThreadPool<T>::ThreadPool(int min, int max)
{
	//实例化任务队列
	do
	{
		taskQ = new TaskQueue<T>;
		//根据线程的最大上限给线程数组分配内存
		threadIDs = new pthread_t[max];
		if (threadIDs == nullptr||taskQ==nullptr) 
		{
			cout << "new threadIDs or new taskQ fail..." << endl;
			break;
		}
		//初始化
		memset(threadIDs, 0, sizeof(pthread_t) * max);
		minNum = min;
		maxNum = max;
		busyNum = 0;
		aliveNum = min;
		exitNum = 0;
		shutDown = false;
		//初始化互斥锁、条件变量
		if (pthread_mutex_init(&mutexPool, NULL) != 0 || pthread_cond_init(&notEmpty, NULL) != 0)
		{
			cout << "mutex or condition init fail...\n";
			break;
		}
		//创建线程
		pthread_create(&managerID, NULL,manager,this );
		for (int i = 0;i<min;++i)
		{
			pthread_create(&threadIDs[i], NULL, worker,this);
			cout << "create threadID is " << to_string(threadIDs[i]) << endl;
		}
	} while (false);
}
template<typename T>
ThreadPool<T>::~ThreadPool()
{
	shutDown = true;
	//销毁管理者线程
	pthread_join(managerID, NULL);
	// 唤醒所有消费者线程
	for (int i = 0; i < aliveNum; ++i)
	{
		pthread_cond_signal(&notEmpty);
	}
	if (taskQ) 
		delete taskQ;
	if (threadIDs) 
		delete[]threadIDs;
	pthread_mutex_destroy(&mutexPool);
	pthread_cond_destroy(&notEmpty);

}
template<typename T>
void ThreadPool<T>::addTask(Task<T> task)
{
	if (shutDown)
	{
		return;
	}
	// 添加任务，不需要加锁，任务队列中有锁
	taskQ->addTask(task);
	// 唤醒工作的线程
	pthread_cond_signal(&notEmpty);

}
template<typename T>
int ThreadPool<T>::getBusyNumber()
{
	int busyNum = 0;
	pthread_mutex_lock(&mutexPool);
	busyNum =this-> busyNum;
	pthread_mutex_unlock(&mutexPool);
	return busyNum;


}
template<typename T>
int ThreadPool<T>::getAliveNumber()
{
	int threadNum = 0;
	pthread_mutex_lock(&mutexPool);
	threadNum =this->aliveNum;
	pthread_mutex_unlock(&mutexPool);
	return threadNum;
}
template<typename T>
void* ThreadPool<T>::worker(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (true)
	{
		pthread_mutex_lock(&pool->mutexPool);
		while (pool->taskQ->taskNumber()==0&&!pool->shutDown)
		{
			cout << "thread" << to_string(pthread_self()) << "waiting..." << endl;
			pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);
			// 解除阻塞之后, 判断是否要销毁线程
			if (pool->exitNum > 0)
			{
				pool->exitNum--;
				if (pool->aliveNum > pool->minNum)
				{
					pool->aliveNum--;
					pthread_mutex_unlock(&pool->mutexPool);
					pool->threadExit();
				}
			}
		}
		// 判断线程池是否被关闭了
		if (pool->shutDown)
		{
			pthread_mutex_unlock(&pool->mutexPool);
			pool->threadExit();
		}
		// 从任务队列中取出一个任务
		Task<T> task = pool->taskQ->takeTask();
		// 工作的线程+1
		pool->busyNum++;
		// 线程池解锁
		pthread_mutex_unlock(&pool->mutexPool);
		// 执行任务
		cout << "thread " << to_string(pthread_self()) << " start working..." << endl;
		task.function(task.arg);
		delete task.arg;
		task.arg = nullptr;

		// 任务处理结束
		cout << "thread " << to_string(pthread_self()) << " end working..." << endl;
		pthread_mutex_lock(&pool->mutexPool);
		pool->busyNum--;
		pthread_mutex_unlock(&pool->mutexPool);
	}
			
	return nullptr;
}
template<typename T>
void* ThreadPool<T>::manager(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	// 如果线程池没有关闭, 就一直检测
	while (!pool->shutDown)
	{
		// 每隔5s检测一次
		sleep(3);
		// 取出线程池中的任务数和线程数量
		//  取出工作的线程池数量
		pthread_mutex_lock(&pool->mutexPool);
		int queueSize = pool->taskQ->taskNumber();
		int liveNum = pool->aliveNum;
		int busyNum = pool->busyNum;
		pthread_mutex_unlock(&pool->mutexPool);

		// 创建线程
		// 当前任务个数>存活的线程数 && 存活的线程数<最大线程个数
		if (queueSize > liveNum && liveNum < pool->maxNum)
		{
			// 线程池加锁
			pthread_mutex_lock(&pool->mutexPool);
			int num = 0;
			for (int i = 0; i < pool->maxNum && num < NUMBER&& pool->aliveNum < pool->maxNum; ++i)
			{
				if (pool->threadIDs[i] == 0)
				{
					pthread_create(&pool->threadIDs[i], NULL, worker, pool);
					num++;
					pool->aliveNum++;
				}
			}
			pthread_mutex_unlock(&pool->mutexPool);
		}

		// 销毁多余的线程
		// 忙线程*2 < 存活的线程数目 && 存活的线程数 > 最小线程数量
		if (busyNum * 2 < liveNum && liveNum > pool->minNum)
		{
			pthread_mutex_lock(&pool->mutexPool);
			pool->exitNum = NUMBER;
			pthread_mutex_unlock(&pool->mutexPool);
			for (int i = 0; i < NUMBER; ++i)
			{
				pthread_cond_signal(&pool->notEmpty);
			}
		}

	}
	return nullptr;
}
template<typename T>
void ThreadPool<T>::threadExit()
{
	pthread_t tid = pthread_self();
	for (int i = 0; i < maxNum; ++i)
	{
		if (threadIDs[i] == tid)
		{
			cout << "threadExit() function: thread "<< to_string(pthread_self()) << " exiting..." << endl;
			threadIDs[i] = 0;
			break;
		}
	}
	pthread_exit(NULL);
}
