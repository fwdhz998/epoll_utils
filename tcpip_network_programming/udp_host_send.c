/*************************************************************************
	> File Name: udp_echo_client.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月18日 星期三 10时16分44秒
  > Description: 演示有边界的UDP数据报
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
	struct sockaddr_in server_addr;
	socklen_t socket_addr_size;
	
	char msg1[] = "Hello";
	char msg2[] = "I am a udp host.";
	char msg3[] = "Nice to meet you.";
	int i;

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

	//连续发送三个UDP数据报
	sendto(client_socket, msg1, strlen(msg1), 0,
				(struct sockaddr*)&server_addr, sizeof(server_addr));
	sendto(client_socket, msg2, strlen(msg2), 0,
				(struct sockaddr*)&server_addr, sizeof(server_addr));
	sendto(client_socket, msg3, strlen(msg3), 0,
				(struct sockaddr*)&server_addr, sizeof(server_addr));

	close(client_socket);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
