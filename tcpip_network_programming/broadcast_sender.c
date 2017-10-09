/*************************************************************************
	> File Name: file_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月29日 星期四 16时27分40秒
  > Description: 
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUFF_SIZE 128

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock_broadcast;
	int sol_broadcast;

	struct sockaddr_in addr_broadcast;

	FILE *fp;
	char buff[BUFF_SIZE];

	if(argc!=3){ //命令行中启动服务程序仅限一个参数：端口号
		printf("Usage : %s <Broadcast IP> <Port>\n", argv[0]);
		exit(1);
	}
	//调用socket函数创建UDP套接字
	sock_broadcast = socket(PF_INET, SOCK_DGRAM, 0);
	if(-1 == sock_broadcast){
		error_handling("socket() error.");
	}
	//打开待传输文件
	fp = fopen("news.txt", "r");
	if(NULL == fp){
		error_handling("Open file failed!");
	}

	sol_broadcast = 1;
	//设置TTL, 协议层为SOL_SOCKET, 选项为SO_BROADCAST
	setsockopt(sock_broadcast, SOL_SOCKET, SO_BROADCAST, (void*)&sol_broadcast, sizeof(sol_broadcast));
	printf("Debug %d\n", 1);
	memset(&addr_broadcast, 0, sizeof(addr_broadcast));
	addr_broadcast.sin_family = AF_INET;
	addr_broadcast.sin_addr.s_addr = inet_addr(argv[1]);// broadcast IP
	addr_broadcast.sin_port = htons(atoi(argv[2])); //broadcast Port
	
	while(!feof(fp)){
		fgets(buff, BUFF_SIZE, fp);
		sendto(sock_broadcast, buff, strlen(buff), 0, (struct sockaddr*)&addr_broadcast, sizeof(addr_broadcast));
		sleep(1);
	}
	//文件传输完毕，关闭文件指针
	
	fclose(fp);
	close(sock_broadcast);
	
	printf("broadcast over!\n");
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
