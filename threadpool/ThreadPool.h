#pragma once
#include"TaskQueue.h"
#include"TaskQueue.cpp"
template<typename T>
class ThreadPool
{
public:
    ThreadPool(int min, int max);
    ~ThreadPool();

    // 添加任务
    void addTask(Task<T> task);
    // 获取忙线程的个数
    int getBusyNumber();
    // 获取活着的线程个数
    int getAliveNumber();

private:
    // 工作的线程的任务函数
    static void* worker(void* arg);
    // 管理者线程的任务函数
    static void* manager(void* arg);
    void threadExit();

private:
    pthread_mutex_t mutexPool;
    pthread_cond_t notEmpty;
    pthread_t* threadIDs;
    pthread_t managerID;
    TaskQueue<T>* taskQ;
    int minNum;
    int maxNum;
    int busyNum;
    int aliveNum;
    int exitNum;
    bool shutdown;
};

