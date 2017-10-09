/*************************************************************************
	> File Name: hello_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月10日 星期二 21时33分01秒
    > Description: Linux Socket网络编程--简单的Server端,该服务器收到连接请求后向请求者应答“Hello world"
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int server_sock;
	int client_sock;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t client_addr_size;

	char message[] = "Hello World!";

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

	//监听端口的连接请求
	if( -1 == listen(server_sock, 5) ){
		error_handling("listen() error");
	}

	//接受连接请求
	client_addr_size = sizeof(client_addr);
	client_sock = accept( server_sock, (struct sockaddr*)&client_addr,
						 &client_addr_size );
	if(-1 == client_sock){
		error_handling("accept() error");
	}

	//建立连接之后的处理逻辑
	write(client_sock, message, sizeof(message));

	//断开连接，关闭套接字
	close(client_sock);
	close(server_sock);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
