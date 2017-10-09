/*************************************************************************
	> File Name: pipe_sample.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月25日 星期三 21时01分27秒
  > Description:
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
	int fds[2];//the argument of function pipe, Used as the file descriptor
	pid_t pid;
	char str[] = "Who are you?";
	char buf[BUF_SIZE];

	int state;
	state = pipe(fds);
	if(state != 0){
		fputs("Error pipe()\n", stderr);
		exit(EXIT_FAILURE);
	}

	pid = fork();
	//复制管道I/O文件描述符，父子进程同时拥有I/O文件描述符
	if(pid < 0){
		fputs("Error fork()\n", stderr);
		exit(1);
	}else if(pid ==0){
		write(fds[1], str, sizeof(str)); 
		//only dfs[1] be used as input of the pipe
	}else{
		str_len = read(fds[0], buf, BUF_SIZE); //only dfs[0] as output
		puts(buf);
	}
	return 0;
}
