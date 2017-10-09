/*************************************************************************
	> File Name: oob_send.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月28日 星期六 15时27分29秒
  > Description:
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>

#define BUFF_SIZE 32
void error_handling(char *message);
int main(int argc, char *argv[])
{
	int sock_client;
	struct sockaddr_in recv_addr;

	if(3 != argc){
		printf("Usage : %s <IP> <Port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	memset(&recv_addr, 0, sizeof(recv_addr));
	recv_addr.sin_family = AF_INET;
	recv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	recv_addr.sin_port = htons(atoi(argv[2]));

	sock_client = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_client < 0){ // == -1
		error_handling("Error socket()");
	}
	if(-1 == connect(sock_client, (struct sockaddr*)&recv_addr, sizeof(recv_addr))){
		error_handling("Error connect()");
	}
	
	write(sock_client, "123", strlen("123"));
	send(sock_client, "45", strlen("45"), MSG_OOB);
	send(sock_client,"9", strlen("9"), MSG_OOB);
	write(sock_client, "Hello", sizeof("Hello"));
	send(sock_client,"0", strlen("0"), MSG_OOB);
	write(sock_client, "789", strlen("789"));
	send(sock_client, "456", strlen("456"), MSG_OOB);
	//似乎只有最后一个MSG_OOB被作为Urgent接收1Byte

	close(sock_client);
	return 0;
}
void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(0);
}
