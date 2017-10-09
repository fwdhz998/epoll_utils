/*************************************************************************
	> File Name: select_sample.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月26日 星期四 10时53分25秒
  > Description: select() 使用示范,监视标准输入,回显输入的内容
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/select.h>

#define BUFF_SIZE 30
int main(int argc, char *argv[])
{
	fd_set reads, reads_init;
	struct timeval timeout, timeout_init;
	char buff[BUFF_SIZE];
	int result, str_len;

	//register fd_set var
	FD_ZERO(&reads_init);
	FD_SET(0, &reads_init); // 0 -- stdin(console)
	//
	timeout_init.tv_sec = 5;
	timeout_init.tv_usec= 0;

	while(1){
		//调用select之后，除发生变化的文件描述符对应的bit，其他所有位置0，所以需用保存初值，通过复制使用
		reads = reads_init;
		//调用select之后，timeval成员值被置为超时前剩余的时间，因此使用时也需要每次用初值重新初始化
		timeout = timeout_init;
		result = select(1, &reads, NULL, NULL, &timeout);
		if(result < 0){
			fputs("Error select()!", stderr);
			break;
		}else if(result == 0){
			puts("Time-out!");
		}else{
			if(FD_ISSET(0, &reads)){
				str_len = read(0, buff, BUFF_SIZE);
				buff[str_len] = 0;
				printf("Message from console: %s", buff);
			}
		}

	}
	return 0;
}
