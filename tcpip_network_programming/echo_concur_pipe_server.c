/*************************************************************************
	> File Name: echo_concurrency_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月25日 星期三 21时19分12秒
  > Description:基于多进程的并发回声服务器
  使用管道将客户端的字符串保存到文件中
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
	//for pipe
	int fds[2];

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
	//create a pipe
	pipe(fds);
	//create a new process to deal with the client
	pid = fork();
	if(pid < 0){
		fputs("Error fork()!", stderr);
		exit(EXIT_FAILURE);
	}else if(pid == 0){ //child process, business logic is here.
		FILE *fp = fopen("log_echo_server.txt", "wt");
		char msgbuf[BUF_SIZE];
		int i, len;
		i = 0;
		do{
			len = read(fds[0], msgbuf, BUF_SIZE);
			fwrite((void*)msgbuf, 1, len, fp);
			i++;
		}while(i<3); //只进行3次read pipe数据操作
		puts("The end of reading pipe.");
		fclose(fp);
	}else{//Parent process
		while(1){
			//接受连接请求
			client_addr_size = sizeof(client_addr);
			client_sock = accept( server_sock, (struct sockaddr*)&client_addr, &client_addr_size );
			//accept函数自动创建数据I/0 socket
			if(-1 == client_sock){
				continue;
			} else{
				puts("new client connected...");
			}

			pid = fork();
			if(pid < 0){
				fputs("Error fork()2.",stderr);
				continue;
			}else if(pid == 0){
				close(server_sock);
				while((str_len = read(client_sock, buff, BUF_SIZE)) > 0){
					write(client_sock, buff, str_len);
					write(fds[1], buff, str_len);
				}
				close(client_sock);
				puts("Client disconnected....");
				exit(0);
			}else{
				close(client_sock);
			}
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
