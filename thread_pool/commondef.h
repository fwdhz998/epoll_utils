//单位秒，监测空闲列表时间间隔，在空闲队列中超过TASK_DESTROY_INTERVAL时间的任务将被自动销毁
const int CHECK_IDLE_TASK_INTERVAL = 300;
//单位秒，任务自动销毁时间间隔
const int TASK_DESTROY_INTERVAL = 60; 

//监控线程池是否为空时间间隔,微秒
const int IDLE_CHECK_POLL_EMPTY = 500;

//线程池线程空闲自动退出时间间隔 ,5分钟
const int  THREAD_WAIT_TIME_OUT = 300;
