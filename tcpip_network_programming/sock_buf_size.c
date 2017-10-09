/*************************************************************************
	> File Name: sock_buf_size.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月20日 星期五 15时05分00秒
  > Description: 读取和设置缓冲区大小
 ************************************************************************/

#include<stdio.h>

#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int tcp_sock;
	int snd_buf, rcv_buf;
	socklen_t opt_len;
	int state;

	tcp_sock = socket(PF_INET, SOCK_STREAM, 0);

	//初始缓冲区大小
	printf("修改前缓冲区大小：\n");

	opt_len = sizeof(snd_buf);
	state = getsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, &opt_len);
	if(state){
		error_handling("send buffer size, getsockopt() error!");
	}
	printf("Input buffer size : %d\n", snd_buf);
	
	opt_len = sizeof(rcv_buf);
	state = getsockopt(tcp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, &opt_len);
	if(state){
		error_handling("receive buffer size, getsockopt() error!");
	}
	printf("Output buffer size : %d\n", rcv_buf);
	//设置新缓冲区size
	snd_buf = 1024*4;
	rcv_buf = 1024*2;
	printf("设置的缓冲区size ： send buffer(%d) recv buffer(%d)\n", snd_buf, rcv_buf);
	
	state = setsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf));
	if(state){
		error_handling("send buffer size, setsockopt() error!");
	}
	
	state = setsockopt(tcp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, sizeof(rcv_buf));
	if(state){
		error_handling("receive buffer size, setsockopt() error!");
	}
	
	//修改后的缓冲区大小
	printf("修改后缓冲区大小 :\n");
	opt_len = sizeof(snd_buf);
	state = getsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, &opt_len);
	if(state){
		error_handling("send buffer size, getsockopt() error!");
	}
	printf("Input buffer size : %d\n", snd_buf);
	
	opt_len = sizeof(rcv_buf);
	state = getsockopt(tcp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, &opt_len);
	if(state){
		error_handling("receive buffer size, getsockopt() error!");
	}
	printf("Output buffer size : %d\n", rcv_buf);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
