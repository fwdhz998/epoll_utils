/*************************************************************************
	> File Name: hello_client.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月11日 星期三 11时09分04秒
  > Description: Linux Socket网络编程--简单的Client端,向服务器端请求建立连接并接收服务器返回的字符串
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
	struct sockaddr_in server_addr;

	char message[30];
	int str_len = 0;

	if(3 != argc){
		printf("Usage : %s <IP> <Port> \n", argv[0]);
		exit(1);
	}

	server_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(-1 == server_sock){
		error_handling("socket() error!");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	if( -1 == connect(server_sock, (struct sockaddr*)&server_addr,
					  sizeof(server_addr)) ){
		error_handling("connect() error!");
	}

	str_len = read(server_sock, message, sizeof(message)-1);
	if(-1 == str_len){
		error_handling("read() error!");
	}
	printf("Message from server : %s \n", message);

	close(server_sock);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
