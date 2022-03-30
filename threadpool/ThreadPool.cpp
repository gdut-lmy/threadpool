#include "ThreadPool.h"

#include<iostream>
#include<cstring>
#include<string>
#include<unistd.h>
using namespace std;
template<typename T>
ThreadPool<T>::ThreadPool(int min, int max)
{
	//ʵ�����������
	do
	{
		taskQ = new TaskQueue<T>;
		//�����̵߳�������޸��߳���������ڴ�
		threadIDs = new pthread_t[max];
		if (threadIDs == nullptr||taskQ==nullptr) 
		{
			cout << "new threadIDs or new taskQ fail..." << endl;
			break;
		}
		//��ʼ��
		memset(threadIDs, 0, sizeof(pthread_t) * max);
		minNum = min;
		maxNum = max;
		busyNum = 0;
		aliveNum = min;
		exitNum = 0;
		shutDown = false;
		//��ʼ������������������
		if (pthread_mutex_init(&mutexPool, NULL) != 0 || pthread_cond_init(&notEmpty, NULL) != 0)
		{
			cout << "mutex or condition init fail...\n";
			break;
		}
		//�����߳�
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
	//���ٹ������߳�
	pthread_join(managerID, NULL);
	// ���������������߳�
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
	// ������񣬲���Ҫ�������������������
	taskQ->addTask(task);
	// ���ѹ������߳�
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
			// �������֮��, �ж��Ƿ�Ҫ�����߳�
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
		// �ж��̳߳��Ƿ񱻹ر���
		if (pool->shutDown)
		{
			pthread_mutex_unlock(&pool->mutexPool);
			pool->threadExit();
		}
		// �����������ȡ��һ������
		Task<T> task = pool->taskQ->takeTask();
		// �������߳�+1
		pool->busyNum++;
		// �̳߳ؽ���
		pthread_mutex_unlock(&pool->mutexPool);
		// ִ������
		cout << "thread " << to_string(pthread_self()) << " start working..." << endl;
		task.function(task.arg);
		delete task.arg;
		task.arg = nullptr;

		// ���������
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
	// ����̳߳�û�йر�, ��һֱ���
	while (!pool->shutDown)
	{
		// ÿ��5s���һ��
		sleep(3);
		// ȡ���̳߳��е����������߳�����
		//  ȡ���������̳߳�����
		pthread_mutex_lock(&pool->mutexPool);
		int queueSize = pool->taskQ->taskNumber();
		int liveNum = pool->aliveNum;
		int busyNum = pool->busyNum;
		pthread_mutex_unlock(&pool->mutexPool);

		// �����߳�
		// ��ǰ�������>�����߳��� && �����߳���<����̸߳���
		if (queueSize > liveNum && liveNum < pool->maxNum)
		{
			// �̳߳ؼ���
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

		// ���ٶ�����߳�
		// æ�߳�*2 < �����߳���Ŀ && �����߳��� > ��С�߳�����
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
