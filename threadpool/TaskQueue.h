#pragma once

#include<queue>
#include <pthread.h>
using callback = void(*)(void*);
struct Task
{
	Task()
	{
		function = nullptr;
		arg = nullptr;
	}
	Task(callback f,void *arg)
	{
		function = f;
		this->arg = arg;
	}
	callback function;
	void* arg;

};
class TaskQueue
{

public:
	TaskQueue();
	~TaskQueue();

	// �������
	void addTask(Task& task);
	void addTask(callback func, void* arg);

	// ȡ��һ������
	Task takeTask();

	// ��ȡ��ǰ�������������
	inline int taskNumber()
	{
		return m_queue.size();
	}

private:
	std::queue<Task> m_queue;
	pthread_mutex_t m_mutex;
};