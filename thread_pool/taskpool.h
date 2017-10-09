#ifndef TASKPOOL_H
#define TASKPOOL_H
/* purpose @ 任务池，主要是缓冲外部高并发任务数，有manager负责调度任务
 *          任务池可自动销毁长时间空闲的Task对象
 *          可通过CHECK_IDLE_TASK_INTERVAL设置检查idle空闲进程轮训等待时间
 *          TASK_DESTROY_INTERVAL 设置Task空闲时间，超过这个时间值将会被CheckIdleTask线程销毁
 * date    @ 2013.12.23
 * author  @ haibin.wang
 */

#include <list>
#include <pthread.h>
#include "commondef.h"

//所有的用户操作为一个task，
typedef void (*task_fun)(void *);
struct Task
{
    task_fun fun; //任务处理函数
    void* data; //任务处理数据
    time_t last_time; //加入空闲队列的时间，用于自动销毁
};

//任务池，所有任务会投递到任务池中，管理线程负责将任务投递给线程池
class TaskPool
{
public:
	/* pur @ 初始化任务池，启动任务池空闲队列自动销毁线程
     * para @ maxSize 最大任务数，大于0
    */ 
    TaskPool(const int & poolMaxSize);
    ~TaskPool();

    /* pur @ 添加任务到任务队列的尾部
     * para @ task， 具体任务
     * return @ 0 添加成功，负数 添加失败
    */    
    int AddTask(task_fun fun, void* arg);
	
    /* pur @ 从任务列表的头获取一个任务
     * return @  如果列表中有任务则返回一个Task指针，否则返回一个NULL
    */    
    Task* GetTask();

    /* pur @ 保存空闲任务到空闲队列中
     * para @ task 已被调用执行的任务
     * return @ 
    */
    void SaveIdleTask(Task*task);
	
    void StopPool();
public:
    void LockIdle();
    void UnlockIdle();
    void CheckIdleWait();
    int RemoveIdleTask();
    bool GetStop();
private:
    static void * CheckIdleTask(void *);
    /* pur @ 获取空闲的task
     * para @ 
     * para @ 
     * return @ NULL说明没有空闲的，否则从m_idleList中获取一个
    */
    Task* GetIdleTask();
    int GetTaskSize();
private:
    int m_poolSize; //任务池大小
    int m_taskListSize; // 统计taskList的大小，因为当List的大小会随着数量的增多而耗时增加
    bool m_bStop; //是否停止
    std::list<Task*> m_taskList;//所有待处理任务列表
    std::list<Task*> m_idleList;//所有空闲任务列表
    pthread_mutex_t m_lock; //对任务列表进行加锁，保证每次只能取一个任务
    pthread_mutex_t m_idleMutex; //空闲任务队列锁
    pthread_cond_t m_idleCond; //空闲队列等待条件
    pthread_t m_idleId;;
};
#endif
