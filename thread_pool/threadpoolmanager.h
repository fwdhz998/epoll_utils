#ifndef THREADPOOLMANAGER_H
#define THREADPOOLMANAGER_H
/* purpose @
 *      基本流程：
 *          管理线程池和任务池，先将任务加入任务池，然后由TaskThread负责从任务池中将任务取出放入到线程池中
 *      基本功能：
 *          1、工作线程可以在业务不忙的时候自动退出部分长时间不使用的线程
 *          2、任务池可以在业务不忙的时候自动释放长时间不使用的资源（可通过commondef.h修改）
 *          3、当程序退时不再向任务池中添加任务，当任务池中所有任务执行完毕后才退出相关程序（做到程序的安全退出）
 *      线程资源：
 *          如果不预分配任何处理线程的话，ThreadPool只有当有任务的时候才实际创建需要的线程，最大线程创建数为用户指定
 *          当manager销毁的时候，manager会创建一个监控所有任务执行完毕的监控线程，只有当所有任务执行完毕后manager才销毁
 *          线程最大数为：1个TaskPool线程 + 1个manager任务调度线程 + ThreadPool最大线程数 + 1个manager退出监控线程 + 1线程池所有线程退出监控线程
 *          线程最小数为：1个TaskPool创建空闲任务资源销毁监控线程 + 1个manager创建任务调度线程
 *      使用方法：
 *          ThreadPoolManager manager;
 *          manager.Init(100000, 50, 5);//初始化一个任务池为10000，线程池最大线程数50，预创建5个线程的管理器
 *          manager.run(fun, data); //添加执行任务到manager中，fun为函数指针，data为fun需要传入的参数，data可以为NULL
 *
 * date    @ 2013.12.23
 * author  @ haibin.wang
 *
 *  详细参数控制可以修改commondef.h中的相关变量值
 */

#include <pthread.h>
typedef void (*task_fun)(void *);

class ThreadPool;
class TaskPool;

class ThreadPoolManager
{
public:
    ThreadPoolManager();
    ~ThreadPoolManager();

    /* pur @ 初始化线程池与任务池，threadPoolMax > threadPoolPre > threadPoolMin >= 0
     * para @ tastPoolSize 任务池大小
     * para @ threadPoolMax 线程池最大线程数
     * para @ threadPoolPre 预创建线程数
     * return @ 0：初始化成功，负数 初始化失败
     *          -1：创建线程池失败
     *          -2：创建任务池失败
     *          -3：线程池初始化失败
    */
    int Init(const int &tastPoolSize,
            const int &threadPoolMax,
            const int &threadPoolPre);

    /* pur @ 执行一个任务
     * para @ fun 需要执行的函数指针
     * para @ arg fun需要的参数，默认为NULL
     * return @ 0 任务分配成功，负数 任务分配失败
     *          -1:任务池满
     *          -2:任务池new失败
     *          -3:manager已经发送停止信号，不再接收新任务
    */
    int Run(task_fun fun,void* arg=NULL);
	
public: //以下public函数主要用于静态函数调用
    bool GetStop();
    void TaskCondWait();
    TaskPool * GetTaskPool();
    ThreadPool * GetThreadPool();
    void LockTask();
    void UnlockTask();
    void LockFull();

private:
	static void * TaskThread(void*); //任务处理线程
	void StopAll();
    
private:
    ThreadPool *m_threadPool; //线程池
    TaskPool * m_taskPool; //任务池
    bool m_bStop; // 是否终止管理器
    
    pthread_t m_taskThreadId; // TaskThread线程id
	pthread_mutex_t m_mutex_task;
    pthread_cond_t m_cond_task;
};
#endif
