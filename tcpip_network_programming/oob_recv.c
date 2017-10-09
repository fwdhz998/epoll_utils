/*************************************************************************
	> File Name: oob_recv.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月28日 星期六 15时48分25秒
  > Description:
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>

#include<signal.h>
#include<netinet/in.h>
#include<fcntl.h>

#define BUFF_SIZE 32
void error_handling(char *msg);
void oob_handler(int signo);

int sock_serv, sock_client;

int main(int argc, char *argv[])
{
	struct sockaddr_in addr_serv;
	socklen_t sz_addr_serv;
	struct sigaction act;
	char buff[BUFF_SIZE];
	ssize_t str_len;
	int state;

	if(2 != argc){
		printf("Usage: %s <Port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	//register signal
	act.sa_handler = oob_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	//set server addr
	memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = PF_INET;
	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_serv.sin_port = htons(atoi(argv[1]));

	sock_serv = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_serv == -1) error_handling("Error socket()");
	if(-1 == bind(sock_serv, (struct sockaddr*)&addr_serv, sizeof(addr_serv))) error_handling("Error bind()");
	if(-1 == listen(sock_serv, 5)) error_handling("Error listen()");

	sz_addr_serv = sizeof(addr_serv);
	sock_client = accept(sock_serv, (struct sockaddr*)&addr_serv, &sz_addr_serv);

	//将文件描述符sock_client指向的套接字拥有者(F_SETOWN)改为ID为getpid()返回值的进程
	fcntl(sock_client, F_SETOWN, getpid());
	//将文件描述符sock_client指向的套接字引发的SIGURG信号处理进程变为ID为..
	state = sigaction(SIGURG, &act, 0);

	while((str_len = recv(sock_client, buff, sizeof(buff), 0)) != 0){
		if(str_len < 0) continue;
		buff[str_len] = 0;
		puts(buff);
	}
	close(sock_client);
	close(sock_serv);
	return 0;
}
void oob_handler(int signo)
{
	int str_len; //ssize_t
	char buff[BUFF_SIZE];
	str_len = recv(sock_client, buff, sizeof(buff)-1, MSG_OOB);
	buff[str_len] = 0;
	printf("Urgent message: %s \n", buff);
}
void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}
