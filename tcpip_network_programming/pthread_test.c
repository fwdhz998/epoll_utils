/*************************************************************************
	> File Name: thread_test.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年04月03日 星期五 15时15分58秒
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

int main(int argc, char *argv[])
{
	pthread_t thread_id[THREAD_NUM];
	int i;

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
	
	printf("Result = %lld\n", num);
	return 0;	
}

void *thread_inc(void *arg)
{
	int i;
	for(i=0; i<50000000; i++){
		num += 1;
	}
	return NULL;
}
void *thread_dec(void *arg)
{
	int i;
	for(i=0; i<50000000; i++){
		num -= 1;
	}
	return NULL;
}
