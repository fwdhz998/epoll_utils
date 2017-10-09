/*************************************************************************
	> File Name: file_client.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月29日 星期四 10时14分21秒
  > Description: 该程序接收数据存在问题，1.最后一个数据报显示两次；2.程序无法退出
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUFF_SIZE 128 //至少不小于sender中的缓冲区大小

void error_handling(char *message);
void debug_print(int no);

int main(int argc, char *argv[])
{
	int sock_recv;
	struct sockaddr_in addr_recv;
	struct ip_mreq join_addr;

	char buff[BUFF_SIZE];
//	FILE *fp;
	int str_len;

	if(3 != argc){
		printf("Usage : %s <GroupIP> <Port> \n", argv[0]);
		exit(1);
	}

	sock_recv = socket(PF_INET, SOCK_DGRAM, 0);
	if(-1 == sock_recv){
		error_handling("socket() error!");
	}
/*
	fp = fopen("received.dat", "wb");
	if(NULL == fp){
		error_handling("File open failed!");
	}
*/
	memset(&addr_recv, 0, sizeof(addr_recv));
	addr_recv.sin_family = AF_INET;
	addr_recv.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_recv.sin_port = htons(atoi(argv[2]));
	
	if(-1 == bind(sock_recv, (struct sockaddr*)&addr_recv, sizeof(addr_recv))) error_handling("Error bind()");

	//设置加入多播组
	join_addr.imr_multiaddr.s_addr = inet_addr(argv[1]); //Group IP
	join_addr.imr_interface.s_addr= htonl(INADDR_ANY); //Host IP
	setsockopt(sock_recv, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_addr, sizeof(join_addr));

	//receive and display the multicast message
	while(1){
		str_len = recvfrom(sock_recv, buff, BUFF_SIZE-1, 0, NULL, 0);
		if(str_len < 0) break;
		buff[str_len] = 0;
		fputs(buff, stdout);
	}//Error:the last packet received twice! && The progress will not exit!

	//fclose(fp);
	puts("File Received from server is over!\n");
	close(sock_recv);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void debug_print(int no)
{
	printf("Debug: %d\n", no);
}
