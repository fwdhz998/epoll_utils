/*************************************************************************
	> File Name: file_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月29日 星期四 9时47分40秒
  > Description: 
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define TTL 64
#define BUFF_SIZE 128

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock_multicast;
	int time_live = TTL;

	struct sockaddr_in addr_multicast;

	FILE *fp;
	char buff[BUFF_SIZE];

	int read_count;

	if(argc!=3){ //命令行中启动服务程序仅限一个参数：端口号
		printf("Usage : %s <IP> <Port>\n", argv[0]);
		exit(1);
	}
	//调用socket函数创建UDP接字
	sock_multicast = socket(PF_INET, SOCK_DGRAM, 0);
	if(-1 == sock_multicast){
		error_handling("socket() error.");
	}
	//打开待传输文件
	fp = fopen("news.txt", "rb");
	if(NULL == fp){
		error_handling("Open file failed!");
	}
	//设置TTL, 协议层为IPPROTO_IP, 选项为IP_MULTICAST_TTL
	setsockopt(sock_multicast, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_live, sizeof(time_live));
	printf("Debug %d\n", 1);
	memset(&addr_multicast, 0, sizeof(addr_multicast));
	addr_multicast.sin_family = AF_INET;
	addr_multicast.sin_addr.s_addr = inet_addr(argv[1]);// Multicast IP
	addr_multicast.sin_port = htons(atoi(argv[2])); //Multicast Port
	
	while(!feof(fp)){
		fgets(buff, BUFF_SIZE, fp);
		sendto(sock_multicast, buff, strlen(buff), 0, (struct sockaddr*)&addr_multicast, sizeof(addr_multicast));
		sleep(1);
	}
	//文件传输完毕，关闭文件指针
	
	fclose(fp);
	close(sock_multicast);
	
	printf("Multicast over!\n");
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
