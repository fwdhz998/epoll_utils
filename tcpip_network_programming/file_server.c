/*************************************************************************
	> File Name: file_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月19日 星期四 14时37分40秒
  > Description: 基于半关闭的文件传输程序，服务器端向建立连接的客户端传输一个文件，并断开输出流，通过输入流接收客户端文件接收完毕消息。
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 30

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int server_sock;
	int client_sock;

	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_size;

	FILE *fp;
	char buf[BUF_SIZE];

	int read_count;

	if(argc!=2){ //命令行中启动服务程序仅限一个参数：端口号
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	//打开待传输文件
	fp = fopen("file_server.c", "rb");
	if(NULL == fp){
		error_handling("Open file failed!");
	}
	//调用socket函数创建TCP接字
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
	}

	while(1){
		read_count = fread((void*)buf, 1, BUF_SIZE, fp);
		if(read_count < BUF_SIZE){
			write(client_sock, buf, read_count);
			break;
		}
		write(client_sock, buf, BUF_SIZE);
	}
	//文件传输完毕，关闭文件指针
	fclose(fp);

	//断开输出流，半断开TCP套接字
	shutdown(client_sock, SHUT_WR);
	
	//接收客户端成功接收完文件之后返回的信息
	read_count = read(client_sock, buf, BUF_SIZE);
	buf[read_count] = 0;
	printf("Message from client : %s\n", buf);

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
