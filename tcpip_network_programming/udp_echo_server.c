/*************************************************************************
	> File Name: udp_echo_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月18日 星期三 09时58分01秒
  > Description: 基于UDP的回声服务器端
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
	int server_socket;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_size;
	
	char message[BUF_SIZE];
	int str_len;

	if(2 != argc){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	server_socket = socket(PF_INET, SOCK_DGRAM, 0);
	if(-1 == server_socket){
		error_handling("UDP socket creation error.");
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if(-1 == bind(server_socket, (struct sockaddr*)&server_addr,
				sizeof(server_addr))){
		error_handling("bind() error!");
	}

	while(1){
		client_addr_size = sizeof(client_addr);
		//接收数据报
		str_len = recvfrom(server_socket, message, BUF_SIZE, 0,
				(struct sockaddr*)&client_addr, &client_addr_size);
		//在屏幕上显示接收的内容
		message[str_len] = 0;
		printf("Message from client is : %s", message);
		//将客户端发来的信息回传给客户端
		sendto(server_socket, message, str_len, 0,
				(struct sockaddr*)&client_addr, client_addr_size);
	}

	close(server_socket);
	return 0;
}


void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
