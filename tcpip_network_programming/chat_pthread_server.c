/*************************************************************************
	> File Name: chat_pthread_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年04月03日 星期五 15时58分53秒
  > Description:基于多线程的聊天程序--服务器端
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>

#define BUFF_SIZE 100 
#define CLIENT_MAX 256 

void error_handling(char *message);
void *client_handler(void *arg);
void send_msg(char *msg, int len);

int count_client = 0;
int socks_client[CLIENT_MAX];
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
	int sock_server;
	int sock_client;

	struct sockaddr_in addr_server;
	struct sockaddr_in addr_client;
	socklen_t size_addr_client;

	pthread_t pthread_id;	

	if(argc!=2){ //命令行中启动服务程序仅限一个参数：端口号
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	pthread_mutex_init(&mutex, NULL);
	//调用socket函数创建套接字
	sock_server = socket(PF_INET, SOCK_STREAM, 0);
	if(-1 == sock_server){
		error_handling("socket() error.");
	}

	memset(&addr_server, 0, sizeof(addr_server));
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_server.sin_port = htons(atoi(argv[1]));
	
	//调用bind函数分配IP地址和端口号
	if( -1 == bind( sock_server, (struct sockaddr*)&addr_server, 
				sizeof(addr_server)) ){
		error_handling("bind() error");
	}

	//监听端口的连接请求,连接请求等待队列size为5
	if( -1 == listen(sock_server, 5) ){
		error_handling("listen() error");
	}

	//
	while(1){
		size_addr_client = sizeof(addr_client);
		sock_client = accept( sock_server, (struct sockaddr*)&addr_client, &size_addr_client);

		pthread_mutex_lock(&mutex);
		socks_client[count_client++] = sock_client;//Lock 访问公共代码区
		pthread_mutex_unlock(&mutex);

		pthread_create(&pthread_id, NULL, client_handler, (void *)&sock_client);
		pthread_detach(pthread_id);//销毁线程
		printf("Client IP:%s \n", inet_ntoa(addr_client.sin_addr));

	}//end of while 

	//断开连接，关闭套接字
	close(sock_server);
	pthread_mutex_destroy(&mutex);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}
void *client_handler(void *arg)
{
	int sock_client = *((int *)arg);
	int str_len = 0, i;
	char msg[BUFF_SIZE];

	while((str_len = read(sock_client, msg, BUFF_SIZE-1)) > 0){
		//msg[str_len] = 0;
		//printf("Received a messgae: %s", msg);
		send_msg(msg, str_len);
	}

	pthread_mutex_lock(&mutex);
	for(i=0; i<count_client; i++){//remove disconnencted client
		if(sock_client == socks_client[i]){
			while(i++ <= count_client-1){
				socks_client[i] = socks_client[i+1];
			}
			break;
		}
	}
	count_client--;
	pthread_mutex_unlock(&mutex);
	close(sock_client);

	return NULL;
}
void send_msg(char *msg, int len)//send message to all clients
{
	int i;
	pthread_mutex_lock(&mutex);
	for(i=0; i<count_client; i++){
		write(socks_client[i], msg, len);
	}
	pthread_mutex_unlock(&mutex);
}
