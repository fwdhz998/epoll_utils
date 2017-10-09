#include "taskpool.h"

#include <string.h>

#include <stdio.h>
#include <pthread.h>

    TaskPool::TaskPool(const int & poolMaxSize)
    : m_poolSize(poolMaxSize)
      , m_taskListSize(0) 
      , m_bStop(false)
{
    pthread_mutex_init(&m_lock, NULL);
    pthread_mutex_init(&m_idleMutex, NULL);
    pthread_cond_init(&m_idleCond, NULL);

    pthread_attr_t attr;
    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE ); // 让线程独立运行
    pthread_create(&m_idleId, &attr, CheckIdleTask, this); //创建监测空闲任务进程 
    pthread_attr_destroy(&attr);
}

TaskPool::~TaskPool()
{
    if(!m_bStop)
    {
        StopPool();
    }
    if(!m_taskList.empty())
    {
        std::list<Task*>::iterator it = m_taskList.begin();
        for(; it != m_taskList.end(); ++it)
        {
            if(*it != NULL)
            {
                delete *it;
                *it = NULL;
            }
        }
        m_taskList.clear();
        m_taskListSize = 0;
    }
    if(!m_idleList.empty())
    {
        std::list<Task*>::iterator it = m_idleList.begin();
        for(; it != m_idleList.end(); ++it)
        {
            if(*it != NULL)
            {
                delete *it;
                *it = NULL;
            }
        }
        m_idleList.clear();
    }


    pthread_mutex_destroy(&m_lock);
    pthread_mutex_destroy(&m_idleMutex);
    pthread_cond_destroy(&m_idleCond);
}

void * TaskPool::CheckIdleTask(void * arg)
{
    TaskPool * pool = (TaskPool*)arg;
    while(1)
    {
        pool->LockIdle();
        pool->RemoveIdleTask();
        if(pool->GetStop())
        {
            pool->UnlockIdle();
            break;
        }
        pool->CheckIdleWait();
        pool->UnlockIdle();
    }
}

void TaskPool::StopPool()
{
    m_bStop = true;
    LockIdle();
    pthread_cond_signal(&m_idleCond); //防止监控线程正在等待，而引起无法退出的问题
    UnlockIdle();
    pthread_join(m_idleId, NULL);
}

bool TaskPool::GetStop()
{
    return m_bStop;
}

void TaskPool::CheckIdleWait()
{
    struct timespec timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = time(0) + CHECK_IDLE_TASK_INTERVAL;
    timeout.tv_nsec = 0;
    pthread_cond_timedwait(&m_idleCond, &m_idleMutex, &timeout);
}

int TaskPool::RemoveIdleTask()
{
    int iRet = 0;
    std::list<Task*>::iterator it, next;
    std::list<Task*>::reverse_iterator rit = m_idleList.rbegin();
    time_t curTime = time(0);
    for(; rit != m_idleList.rend(); )
    {
        it = --rit.base();
        if(difftime(curTime,((*it)->last_time)) >= TASK_DESTROY_INTERVAL)
        {
            iRet++;
            delete *it;
            *it = NULL;
            next = m_idleList.erase(it);
            rit = std::list<Task*>::reverse_iterator(next);
        }
        else
        {
            break;    
        }
    }
}

int TaskPool::AddTask(task_fun fun, void *arg)
{
    int iRet = 0;
    if(0 != fun)
    {
        pthread_mutex_lock(&m_lock);
        if(m_taskListSize >= m_poolSize)
        {
            pthread_mutex_unlock(&m_lock);
            iRet = -1; //task pool is full;
        }
        else
        {
            pthread_mutex_unlock(&m_lock);
            Task * task = GetIdleTask();
            if(NULL == task)
            {
                task = new Task;
            }
            if(NULL == task)
            {
                iRet = -2; // new failed 
            }
            else
            {
                task->fun = fun;
                task->data = arg;
                pthread_mutex_lock(&m_lock);
                m_taskList.push_back(task);
                ++m_taskListSize;
                pthread_mutex_unlock(&m_lock);
            }
        }
    }
    return iRet;
}

Task* TaskPool::GetTask()
{
    Task *task = NULL;
    pthread_mutex_lock(&m_lock);
    if(!m_taskList.empty())
    {
        task =  m_taskList.front();
        m_taskList.pop_front();
        --m_taskListSize;
    }
    pthread_mutex_unlock(&m_lock);
    return task;
}

void TaskPool::LockIdle()
{
    pthread_mutex_lock(&m_idleMutex);
}

void TaskPool::UnlockIdle()
{
    pthread_mutex_unlock(&m_idleMutex);
}

Task * TaskPool::GetIdleTask()
{
    LockIdle();
    Task * task = NULL;
    if(!m_idleList.empty())
    {
        task = m_idleList.front();
        m_idleList.pop_front();
    }
    UnlockIdle();
    return task;
}

void TaskPool::SaveIdleTask(Task*task)
{
    if(NULL != task)
    {
        task->fun = 0;
        task->data = NULL;
        task->last_time = time(0);
        LockIdle();
        m_idleList.push_front(task);
        UnlockIdle();
    }
}
