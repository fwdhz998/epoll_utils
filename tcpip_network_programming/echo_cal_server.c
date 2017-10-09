/*************************************************************************
	> File Name: echo_cal_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月17日 星期二 16时03分01秒
    > Description: Linux Socket网络编程--基于TCP的简单的整数计算服务器
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *message);
int calculate(int operand_num, int operand[], char operator);

#define BUF_SIZE 1024
#define RESULT_SIZE sizeof(int)
#define OPERAND_SIZE sizeof(int)

int main(int argc, char *argv[])
{
	int server_sock;
	int client_sock;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t client_addr_size;

	char message[BUF_SIZE];

	int operand_count, result, i;
	int recv_cnt, recv_len;

	if(argc!=2){ //命令行中启动服务程序仅限一个参数：端口号
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	//调用socket函数创建套接字
	server_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(-1 == server_sock){
		error_handling("socket() error.");
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	
	//调用bind函数分配IP地址和端口号
	if( -1 == bind( server_sock, (struct sockaddr*)&server_addr, 
				sizeof(server_addr)) ){
		error_handling("bind() error");
	}

	//监听端口的连接请求,连接请求等待队列size为5
	if( -1 == listen(server_sock, 5) ){
		error_handling("listen() error");
	}

	//接受连接请求
	client_addr_size = sizeof(client_addr);

	for(i=0; i<5; i++){
		client_sock = accept( server_sock, (struct sockaddr*)&client_addr,
						 &client_addr_size );
		//accept函数自动创建数据I/0 socket
		if(-1 == client_sock){
			error_handling("accept() error");
		} else{
			printf("Connected client : %d\n", i+1);
		}
		
		//read(client_sock, &operand_count, 1);
		//无法直接获取正确的operand_count，需用通过如下两个语句替换
		read(client_sock, &message[BUF_SIZE-1], 1);
		operand_count = (int)message[BUF_SIZE-1];
		//接收第一个字节，获取操作数个数
		printf("Operand Num : %d\n", operand_count);
		
		recv_len = 0;
		while( recv_len < (operand_count*OPERAND_SIZE + 1) ){
			recv_cnt = read(client_sock, &message[recv_len], BUF_SIZE-recv_len);
			recv_len += recv_cnt;
		}
		//printf("Received message length : %d\n", recv_len);
		
		result = calculate(operand_count, (int *)message, message[operand_count*OPERAND_SIZE]);
		//printf("Result : %d\n", result);		

		write(client_sock, &result, RESULT_SIZE);

		close(client_sock); //关闭accept函数调用返回时自动创建的socket
	}

	//断开连接，关闭套接字
	close(server_sock);

	return 0;
}

int calculate(int operand_num, int operand[], char operator)
{
	int i, result;
	switch(operator){
		case '+':
			result = 0;
			for(i=0; i<operand_num; i++) result += operand[i];
			break;
		case '-':
			result = operand[0];
			for(i=1; i<operand_num; i++) result -= operand[i];
			break;
		case '*':
			result = 1;
			for(i=0; i<operand_num; i++) result *= operand[i];
			break;
		default:
			return 0;
	}
	return result;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
