/*************************************************************************
	> File Name: udp_connected_echo_client.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月18日 星期三 14时46分44秒
  > Description: 基于UDP的简单连接的回声客户端
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
	int client_socket;
	struct sockaddr_in server_addr, from_addr;
	socklen_t socket_addr_size;
	
	char message[BUF_SIZE];
	int str_len;

	if(3 != argc){
		printf("Usage : %s <ip> <port>\n", argv[0]);
		exit(1);
	}

	client_socket = socket(PF_INET, SOCK_DGRAM, 0);
	if(-1 == client_socket){
		error_handling("UDP socket creation error.");
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	//建立连接,实质是向UDP套接字注册目标IP和端口信息
	connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

	while(1){
		fputs("Input message(Q to quit) : ", stdout);
		fgets(message, sizeof(message), stdin);
		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n")) break;
		/*
		sendto(client_socket, message, strlen(message), 0,
				(struct sockaddr*)&server_addr, sizeof(server_addr));
		*/
		write(client_socket, message, strlen(message));
		/*
		socket_addr_size = sizeof(from_addr);
		str_len = recvfrom(client_socket, message, BUF_SIZE, 0,
				(struct sockaddr*)&from_addr, &socket_addr_size);
		*/
		str_len = read(client_socket, message, sizeof(message)-1);

		message[str_len] = 0;
		printf("Message from server: %s", message);
	}

	close(client_socket);
	return 0;
}


void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
