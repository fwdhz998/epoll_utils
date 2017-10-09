/*************************************************************************
	> File Name: echo_cal_client.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月17日 星期三 15时39分04秒
	> Description: Linux Socket网络编程--基于TCP的简单的回声计算整数Client端,向服务器端请求建立连接并发送待计算的整数与操作，接收服务器返回的计算结果。
	应用层协议：
		客户端连接到服务器端以1字节整数形式传递待运算的整数个数
		客户端向服务器端传递的每个整型数据占用一个机器字空间(int)
		客户端传递整数之后传递运算符(+，-，*)，运算符占用1个字节
		服务器端一个机器字大小的整数向客户端返回结果
		客户端得到结果之后与服务器端断开连接
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *message);

#define BUF_SIZE 1024
#define RESULT_SIZE sizeof(int)
#define OPERAND_SIZE sizeof(int)

int main(int argc, char *argv[])
{
	int server_sock;
	struct sockaddr_in server_addr;

	char message[BUF_SIZE];
	int operand_count, result, i;

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

	printf("Operand count : ");
	scanf("%d", &operand_count);
	message[0] = (char)operand_count;

	for(i=0; i<operand_count; i++){
		printf("Operand %d : ", i+1);
		scanf("%d", (int *)&message[i*OPERAND_SIZE+1]);
	}
	fgetc(stdin);
	fputs("Operator(+ or - or *) : ", stdout);
	//需要使用上面两个语句替换下面的这一行语句，否则for循环执行完毕程序暂停
	//printf("Operator(+ or - or *) : ");
	scanf("%c", &message[operand_count*OPERAND_SIZE+1]);
	
	write(server_sock, message, operand_count*OPERAND_SIZE+2);

	read(server_sock, &result, RESULT_SIZE);
	printf("Result from server : %d \n", result);

	close(server_sock);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
