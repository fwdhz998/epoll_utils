/*************************************************************************
	> File Name: wait.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月25日 星期三 14时10分42秒
  > Description:使用wait函数避免zombie
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

#define BUF_SIZE 30

void error_handling(char *message);

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;

	pid = fork();

	if(pid < 0){
		error_handling("Error fork()!");
		exit(EXIT_FAILURE);
	}else if(pid == 0){
		printf("Hello, I am a child process.\n");
	}else{
		printf("I am the parent process.\n");
		//sleep(10); //无法避免zombie
		wait(&status);
		if(WIFEXITED(status)){
			printf("Return value of child: %d\n", WEXITSTATUS(status));
		}
		sleep(10);
	}

	if(pid == 0){
		printf("Child Process: End.\n");
		return 123;
	}else{
		printf("Parent process: End.\n");
	}

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
