#pragma once
#include"TaskQueue.h"
#include"TaskQueue.cpp"
template<typename T>
class ThreadPool
{
public:
    ThreadPool(int min, int max);
    ~ThreadPool();

    // �������
    void addTask(Task<T> task);
    // ��ȡæ�̵߳ĸ���
    int getBusyNumber();
    // ��ȡ���ŵ��̸߳���
    int getAliveNumber();

private:
    // �������̵߳�������
    static void* worker(void* arg);
    // �������̵߳�������
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

