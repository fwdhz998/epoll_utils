#include <iostream>
#include <string>
#include "threadpoolmanager.h"
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


using namespace std;
int seq = 0;
int billNum =0;
int inter = 1;
pthread_mutex_t m_mutex;
void myFunc(void*arg)
{
    pthread_mutex_lock(&m_mutex);
    seq++;
    if(seq%inter == 0 )
    {
        cout << "fun 1=" << seq << endl;
    }
    if(seq>=1000000000)
    {
        cout << "billion" << endl;
        seq = 0;
        billNum++;
    }
    pthread_mutex_unlock(&m_mutex);
    //sleep();
}

int main(int argc, char** argv)
{
    if(argc != 6)
    {
        cout << "必须有5个参数 任务执行次数 任务池大小 线程池大小 预创建线程数 输出间隔" << endl;
        cout << "eg: ./test 999999 10000 100 10 20" << endl;
        cout << "上例代表创建一个间隔20个任务输出，任务池大小为10000，线程池大小为100，预创建10个线程，执行任务次数为：999999" << endl;
        return 0;
    }
    double loopSize = atof(argv[1]);
    int taskSize = atoi(argv[2]);
    int threadPoolSize = atoi(argv[3]);
    int preSize = atoi(argv[4]);
    inter = atoi(argv[5]);

    pthread_mutex_init(&m_mutex,NULL);
    ThreadPoolManager manager;
    if(0>manager.Init(taskSize,  threadPoolSize, preSize))
    {
        cout << "初始化失败" << endl;
        return 0;
    }
    cout << "*******************初始化完成*********************" << endl;
    struct timeval time_beg, time_end;
    memset(&time_beg, 0, sizeof(struct timeval));
    memset(&time_end, 0, sizeof(struct timeval)); 
    gettimeofday(&time_beg, NULL);
    double i=0;
    for(; i<loopSize; ++i)
    {
        while(0>manager.Run(myFunc,NULL))
        {
            usleep(100);
        }
    }
    gettimeofday(&time_end, NULL);
    long total = (time_end.tv_sec - time_beg.tv_sec)*1000000 + (time_end.tv_usec - time_beg.tv_usec);
    cout << "total time =" << total << endl;
    cout << "total num =" << i  << " billion num=" << billNum<< endl;
    cout << __FILE__ << "将关闭所有线程" << endl;
    //pthread_mutex_destroy(&m_mutex);
    return 0;
}
