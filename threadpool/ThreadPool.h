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
    // �߳��˳�����
    void threadExit();

private:
    pthread_mutex_t mutexPool;//���������̳߳�
    pthread_cond_t notEmpty;//ά��������е���������


    pthread_t* threadIDs;//�����̵߳�����
    pthread_t managerID;//�������߳�
    TaskQueue<T>* taskQ;//�������

    static const int NUMBER = 2;//������ÿ������̵߳�����

    int minNum;//��С�߳�����
    int maxNum;//����߳�����
    int busyNum;//æ�߳�����
    int aliveNum;//�����߳�����
    int exitNum;//Ҫ���ٵ��߳�����
    bool shutDown;//�Ƿ������̳߳�
};

