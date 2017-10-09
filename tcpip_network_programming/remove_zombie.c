/*************************************************************************
	> File Name: sigaction.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月25日 星期三 15时10分00秒
  > Description:Remove the zombie process by sigaction.
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h>

void error_handling(char *message);
void read_childproc(int sig)
{
	int status;
	pid_t pid;
	pid = waitpid(-1, &status, WNOHANG);
	if(WIFEXITED(status)){
		printf("Remove child process : PID = %d\n", pid);
		printf("Return value of child: %d\n", WEXITSTATUS(status));
	}
}

int main(int argc, char *argv[])
{
	int i;
	pid_t pid;
	struct sigaction sig_act;

	sig_act.sa_handler = read_childproc;
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;

	sigaction(SIGCHLD, &sig_act, 0);

	pid = fork();

	if(pid < 0){
		error_handling("Error fork()");
	}else if(pid == 0){
		puts("Hi, I am child process!");
		sleep(3);
		return 123;
	}else{
		printf("Parent : child pid = %d\n", pid);
		for(i=0; i<3; i++){
			puts("Parent: waiting....");
			sleep(2);
		}
	}
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
