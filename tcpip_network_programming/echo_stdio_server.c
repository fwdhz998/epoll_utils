/*************************************************************************
	> File Name: echo_stdio_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月30日 星期一 14时29分04秒
  > Description:使用标准I/O实现回声服务器
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
	FILE *fp_write, *fp_read;

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

		fp_read = fdopen(client_sock, "r");
		fp_write = fdopen(client_sock, "w");
		/*
		while( (str_len = read(client_sock, message, BUF_SIZE)) != 0 ){
			write(client_sock, message, str_len);
		}
		close(client_sock); //关闭accept函数调用返回时自动创建的socket
		*/
		while(!feof(fp_read)){
			//调用基于字符串的fgets, fputs函数提供服务
			fgets(message, BUF_SIZE, fp_read);
			fputs(message, fp_write);
			fflush(fp_write);
			//standard I/O 内部提供额外的buffer，因此需用使用fflush()确保立即发送数据至套接字输出缓冲区
		}
		fclose(fp_read);
		fclose(fp_write);
	}

	//关闭 listen 套接字
	close(server_sock);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
