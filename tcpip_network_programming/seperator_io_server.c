/*************************************************************************
	> File Name: seperator_io_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月31日 星期二 09时35分53秒
  > Description:
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *message);

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	int sock_server;
	int sock_client;
	FILE *fp_write, *fp_read;

	struct sockaddr_in addr_server;
	struct sockaddr_in addr_client;
	socklen_t addr_client_size;

	char message[BUFF_SIZE];

	if(argc!=2){ //命令行中启动服务程序仅限一个参数：端口号
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	//调用socket函数创建套接字
	sock_server = socket(PF_INET, SOCK_STREAM, 0);
	if(-1 == sock_server){
		error_handling("socket() error.");
	}

	memset(&addr_server, 0, sizeof(addr_server));
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_server.sin_port = htons(atoi(argv[1]));
	
	//调用bind函数分配IP地址和端口号
	if( -1 == bind( sock_server, (struct sockaddr*)&addr_server, 
				sizeof(addr_server)) ){
		error_handling("bind() error");
	}

	//监听端口的连接请求,连接请求等待队列size为5
	if( -1 == listen(sock_server, 5) ){
		error_handling("listen() error");
	}

	//接受连接请求
	addr_client_size = sizeof(addr_client);

	sock_client = accept( sock_server, (struct sockaddr*)&addr_client,
						 &addr_client_size );
	//accept函数自动创建数据I/0 socket
	if(-1 == sock_client){
		error_handling("accept() error");
	} else{
		printf("Connected client... \n");
	}

	fp_read = fdopen(sock_client, "r");
	fp_write = fdopen(dup(sock_client), "w");//基于创建的文件描述符复制
	//调用基于字符串的fgets, fputs函数提供服务
	fputs("FROM SERVER: Hi~ client? \n", fp_write);
	fputs("I love you.\n", fp_write);
	fputs("You are awesome! \n", fp_write);
	fflush(fp_write);

	//调用shutdown函数时无论复杂多少文件描述符都进入半关闭状态，同时传递EOF
	shutdown(fileno(fp_write), SHUT_WR);//半关闭套接字，发送EOF
	fclose(fp_write);

	fgets(message, BUFF_SIZE, fp_read);
	fputs(message, stdout);
	fclose(fp_read);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
