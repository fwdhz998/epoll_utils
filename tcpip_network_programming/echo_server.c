/*************************************************************************
	> File Name: echo_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月17日 星期二 14时33分01秒
    > Description: Linux Socket网络编程--基于TCP的简单的迭代回声服务器
	服务器端在同一时刻只与一个客户端连接，并提供回声服务；
	服务器依次向5个客户端提供服务并退出；
	客户端接受用户输入的字符串并发送到服务器端；
	服务器端将接收的字符串传回客户端，即“回声”；
	服务器端与客户端之间的回声一直执行到客户端输入Q。
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *message);

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	int server_sock;
	int client_sock;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t client_addr_size;

	char message[BUF_SIZE];

	int str_len, i;

	if(argc!=2){ //命令行中启动服务程序仅限一个参数：端口号
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
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

	//接受连接请求
	client_addr_size = sizeof(client_addr);

	for(i=0; i<5; i++){
		client_sock = accept( server_sock, (struct sockaddr*)&client_addr,
						 &client_addr_size );
		//accept函数自动创建数据I/0 socket
		if(-1 == client_sock){
			error_handling("accept() error");
		} else{
			printf("Connected client : %d\n", i+1);
		}
		while( (str_len = read(client_sock, message, BUF_SIZE)) != 0 ){
			write(client_sock, message, str_len);
		}
		close(client_sock); //关闭accept函数调用返回时自动创建的socket
	}

	//断开连接，关闭套接字
	close(server_sock);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
