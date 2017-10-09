#ifndef THREADPOOL_H
#define THREADPOOL_H
/* purpose @ 线程池类，负责线程的创建与销毁，实现线程超时自动退出功能(半驻留)a
 *          当线程池退出时创建TerminalCheck线程，负责监测线程池所有线程退出
 * date    @ 2013.12.23
 * author  @ haibin.wang
 */

#include <list>
#include <string>
#include "taskpool.h"
//通过threadmanager来控制任务调度进程
//threadpool的TerminalCheck线程负责监测线程池所有线程退出


class ThreadPool;
class Thread
{
public:
    Thread(bool detach, ThreadPool * pool);
    ~Thread();
    pthread_t  m_threadId; //线程id
    pthread_mutex_t m_mutex; //互斥锁
    pthread_cond_t m_cond; //条件变量
    pthread_attr_t m_attr; //线程属性
	Task  task; // 
    ThreadPool * m_pool; //所属线程池
};

//线程池，负责创建线程处理任务，处理完毕后会将线程加入到空闲队列中,从任务池中
class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();

    /* pur @ 初始化线程池
     * para @ poolMax 线程池最大线程数
     * para @ poolPre 预创建线程数
     * return @ 0:成功
     *          -1: parameter error, must poolMax > poolPre >=0
     *          -2: 创建线程失败
    */
    int InitPool(const int & poolMax, const int & poolPre);

    /* pur @ 执行一个任务
     * para @ task 任务指针
     * return @ 0任务分配成功，负值 任务分配失败，-1，创建新线程失败
    */
    int Run(task_fun fun, void* arg);
	
	/* pur @ 设置是否停止线程池工作
     * para @ bStop true停止，false不停止
    */
	void StopPool(bool bStop);
    
public: //此公有函数主要用于静态函数调用
    /* pur @ 获取进程池的启停状态
     * return @ 
    */
    bool GetStop();    
	void SaveIdleThread(Thread * thread );
    void LockMutex();
    void UnlockMutex();
    void DecreaseTotalNum();
    void IncreaseTotalNum();
    void RemoveThread(Thread* thread);
    void TerminalCondSignal();
    int TotalThreads();
    void SendSignal();
private:
	/* pur @ 创建线程
     * return @ 非空 成功，NULL失败，
    */
	Thread * CreateThread();

    /* pur @ 从线程池中获取一个一个线程运行任务
     * para @ fun 函数指针
     * para @ arg 函数参数
     * return @ 
    */
    void GetThreadRun(task_fun fun, void* arg);

	static void * WapperFun(void*);
	static void * TerminalCheck(void*);//循环监测是否所有线程终止线程

private:
    int m_poolMax;//线程池最大线程数
    int m_idleNum; //空闲线程数
    int m_totalNum; //当前线程总数 小于最大线程数	
	bool m_bStop; //是否停止线程池
	pthread_mutex_t m_mutex; //线程列表锁
	pthread_mutex_t m_runMutex; //run函数锁

    pthread_mutex_t m_terminalMutex; //终止所有线程互斥量
    pthread_cond_t  m_terminalCond; //终止所有线程条件变量
    pthread_cond_t  m_emptyCond; //空闲线程不空条件变量
	
    std::list<Thread*> m_threads; // 线程列表
};
#endif
