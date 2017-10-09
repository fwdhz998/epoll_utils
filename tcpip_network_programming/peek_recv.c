/*************************************************************************
	> File Name: echo_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月29日 星期二 8时47分01秒
    > Description: Linux Socket网络编程--recv,验证输入缓冲，MSG_PEEK MSG_DONTWAIT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *message);

#define BUF_SIZE 32

int main(int argc, char *argv[])
{
	int server_sock;
	int client_sock;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t client_addr_size;

	char message[BUF_SIZE];

	int str_len;

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
	client_sock = accept( server_sock, (struct sockaddr*)&client_addr,
						 &client_addr_size );
	//accept函数自动创建数据I/0 socket
	if(-1 == client_sock){
		error_handling("accept() error");
	} else{
		printf("Connected client!\n");
	}

	while(1){
		str_len = recv(client_sock, message, sizeof(message)-1,
						MSG_PEEK | MSG_DONTWAIT);
		//设置PEEK选项并调用recv函数时，即使读取量输入缓冲中的数据也不会删除
		//MSG_DONTWAIT 以非阻塞方式验证待读数据存在与否
		if(str_len > 0) break;
	}

	message[str_len] = 0;
	printf("Buffering %lu bytes: %s\n", sizeof(message), message);

	str_len = 0;
	memset(message, 0, BUF_SIZE);
	str_len = recv(client_sock, message, sizeof(message)-1, 0);
	message[str_len] = 0;
	printf("Received %lu bytes: %s\n", sizeof(message), message);

	close(client_sock); //关闭accept函数调用返回时自动创建的socket
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
