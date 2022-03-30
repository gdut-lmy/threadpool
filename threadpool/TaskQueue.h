#pragma once

#include<queue>
#include <pthread.h>
using callback = void(*)(void*);
template<typename T>
struct Task
{
	Task<T>()
	{
		function = nullptr;
		arg = nullptr;
	}
	Task<T>(callback f,void *arg)
	{
		function = f;
		this->arg = (T *)arg;
	}
	callback function;
	T* arg;

};
template<typename T>
class TaskQueue
{

public:
	TaskQueue();
	~TaskQueue();

	// 添加任务
	void addTask(Task<T>& task);
	void addTask(callback func, void* arg);

	// 取出一个任务
	Task<T> takeTask();

	// 获取当前队列中任务个数
	inline size_t taskNumber()
	{
		return m_queue.size();
	}

private:
	std::queue<Task<T>> m_queue;
	pthread_mutex_t m_mutex;
};