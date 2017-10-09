/*************************************************************************
	> File Name: thread_mutex_test.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年04月03日 星期五 15时45分58秒
  > Description:
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

#define THREAD_NUM 30

void * thread_inc(void *arg);
void * thread_dec(void *arg);

long long num = 0;
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
	pthread_t thread_id[THREAD_NUM];
	int i;

	pthread_mutex_init(&mutex,NULL);

	printf("sizeof(long long) = %lu\n", sizeof(long long));
	for(i=0; i<THREAD_NUM; i++){
		if(i%2){
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
		}else{
			pthread_create(&(thread_id[i]), NULL, thread_dec, NULL);
		}
		//pthread_join(thread_id[i], NULL); //The result is OK.
	}
	for(i=0; i<THREAD_NUM; i++){
		pthread_join(thread_id[i], NULL);
	}
	
	pthread_mutex_destroy(&mutex);

	printf("Result = %lld\n", num);
	return 0;	
}

void *thread_inc(void *arg)
{
	int i;
	pthread_mutex_lock(&mutex);
	for(i=0; i<50000000; i++){
		num += 1;
	}
	pthread_mutex_unlock(&mutex);
	return NULL;
}
//两个函数临界区划分范围不同，thread_inc划分范围大，但是其它线程等待时间长；thread_dec划分范围小，但是lock、unlock函数调用那个次数多----需要折中
void *thread_dec(void *arg)
{
	int i;
	for(i=0; i<50000000; i++){
		pthread_mutex_lock(&mutex);
		num -= 1;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
