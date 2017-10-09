/*************************************************************************
	> File Name: echo_concurIO_client.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月25日 星期三 16时50分55秒
  > Description:通过多进程分割客户端的I/O
  在功能复杂的程序中，可以简化收发数据的处理逻辑；
  可以提高频繁交换数据的程序性能，这在网速较慢的环境中尤其明显
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *message);
void read_routine(int sock);
void write_routine(int sock);

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	int server_sock;
	struct sockaddr_in server_addr;

	int str_len, recv_len, recv_cnt;

	pid_t pid;

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
	}else{
		puts("Connected......");
	}

	//分割I/O
	pid = fork();
	if(pid < 0){
		error_handling("Error fork()");
	}else if(pid == 0){
		printf("Prompt: Input message at anytime(Q to quit).\n");
		read_routine(server_sock);
	}else{
		write_routine(server_sock);
	}

	close(server_sock);

	return 0;
}

void read_routine(int sock)
{
	char buff[BUF_SIZE];
	int str_len;

	while(1){
		str_len = read(sock, buff, BUF_SIZE);
		if(str_len < 1){
			return;
		}
		buff[str_len] = 0;
		printf("Message from server : %s \n", buff);
	}

}

void write_routine(int sock)
{
	char buff[BUF_SIZE];

	while(1){
		//printf("Input message(Q to quit) : ");
		fgets(buff, BUF_SIZE, stdin);

		if( 0 == strcmp(buff, "q\n") || 0 == strcmp(buff, "Q\n") ){
			shutdown(sock, SHUT_WR);
			//Due to fork(), two socket file descriptor are existed. Only one close() will not send 'EOF'. So shutdown() is needed.
			return;
		}

		write(sock, buff, strlen(buff));
	}

}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
