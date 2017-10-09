/*************************************************************************
	> File Name: echo_concurrency_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月25日 星期三 16时19分12秒
  > Description:基于多进程的并发回声服务器
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<arpa/inet.h>
#include<sys/socket.h>

#include<sys/wait.h>
#include<signal.h>

void error_handling(char *message);

#define BUF_SIZE 1024
void read_childproc(int sig);

int main(int argc, char *argv[])
{
	int server_sock;
	int client_sock;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t client_addr_size;

	char buff[BUF_SIZE];
	int str_len, i;

	pid_t pid;
	struct sigaction sig_act;

	if(argc!=2){ //命令行中启动服务程序仅限一个参数：端口号
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	//register sig to remove zombie process
	sig_act.sa_handler = read_childproc;
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sigaction(SIGCHLD, &sig_act, 0);
	
	//调用socket函数创建套接字
	server_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(-1 == server_sock){
		error_handling("socket() error.");
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	
	//调用bind函数分配IP地址和端口号
	if( -1 == bind( server_sock, (struct sockaddr*)&server_addr, 
				sizeof(server_addr)) ){
		error_handling("bind() error");
	}

	//监听端口的连接请求,连接请求等待队列size为5
	if( -1 == listen(server_sock, 5) ){
		error_handling("listen() error");
	}

	while(1){
		//接受连接请求
		client_addr_size = sizeof(client_addr);
		client_sock = accept( server_sock, (struct sockaddr*)&client_addr,
						 &client_addr_size );
		//accept函数自动创建数据I/0 socket
		if(-1 == client_sock){
			continue;
		} else{
			puts("new client connected...");
		}
		//create a new process to deal with the client
		pid = fork();
		if(pid < 0){
			fputs("Error fork()!", stderr);
			continue;
		}else if(pid == 0){ //child process, business logic is here.
			close(server_sock);//该server socket 同样通过复制传递给子进程，需用子进程关闭该文件描述符
			while( (str_len = read(client_sock, buff, BUF_SIZE)) != 0 ){
				write(client_sock, buff, str_len);
			}
			close(client_sock); //关闭accept函数调用返回时自动创建的socket
			exit(0); //Child process terminated.
		}else{ //Parent process
			close(client_sock); 
			/*关闭accept函数调用返回时自动创建的socket, 该socket文件描述符已经通过各复制传递给子进程, 服务器端需用销毁自己所拥有的文件描述符。*/
		}
	}

	//断开连接，关闭套接字
	close(server_sock);

	return 0;
}

void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid = waitpid(-1, &status, WNOHANG);
	printf("removed child proc ID = %d\n", pid);
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
