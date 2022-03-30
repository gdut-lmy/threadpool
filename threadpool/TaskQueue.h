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

	// 添加任务
	void addTask(Task& task);
	void addTask(callback func, void* arg);

	// 取出一个任务
	Task takeTask();

	// 获取当前队列中任务个数
	inline int taskNumber()
	{
		return m_queue.size();
	}

private:
	std::queue<Task> m_queue;
	pthread_mutex_t m_mutex;
};