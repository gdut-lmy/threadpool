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
    // 线程退出函数
    void threadExit();

private:
    pthread_mutex_t mutexPool;//锁整个的线程池
    pthread_cond_t notEmpty;//维护任务队列的条件变量


    pthread_t* threadIDs;//创建线程的数组
    pthread_t managerID;//管理者线程
    TaskQueue<T>* taskQ;//任务队列

    static const int NUMBER = 2;//管理者每次添加线程的数量

    int minNum;//最小线程数量
    int maxNum;//最大线程数量
    int busyNum;//忙线程数量
    int aliveNum;//存活的线程数量
    int exitNum;//要销毁的线程数量
    bool shutDown;//是否销毁线程池
};

