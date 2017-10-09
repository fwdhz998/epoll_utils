/*************************************************************************
	> File Name: file_client.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月19日 星期四 14时44分21秒
  > Description: 基于半关闭的文件传输程序客户端，接收服务器端文件之后，发送一个消息。
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
	struct sockaddr_in server_addr;

	char buf[BUF_SIZE], message[BUF_SIZE];
	FILE *fp;
	int read_count = 0;

	if(3 != argc){
		printf("Usage : %s <IP> <Port> \n", argv[0]);
		exit(1);
	}

	fp = fopen("received.dat", "wb");
	if(NULL == fp){
		error_handling("File open failed!");
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

	while((read_count = read(server_sock, message, BUF_SIZE))){
		fwrite((void*)buf, 1, read_count, fp);
	}
	fclose(fp);

	puts("File Received from server.\n");
	//返回消息，若服务器端未断开输入流则可以接收此消息
	strcpy(message, "File Received. Thank you!");
	write(server_sock, message, strlen(message));

	close(server_sock);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
