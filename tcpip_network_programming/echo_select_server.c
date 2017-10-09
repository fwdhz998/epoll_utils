/*************************************************************************
	> File Name: echo_select_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月26日 星期四 14时03分40秒
  > Description:使用select函数实现I/O复用服务器端
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<sys/select.h>

void error_handling(char *message);

#define BUFF_SIZE 32

int main(int argc, char *argv[])
{
	int server_sock;
	int client_sock;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t client_addr_size;

	char buff[BUFF_SIZE];
	fd_set reads, reads_init;
	struct timeval timeout, timeout_init;

	int str_len, i, fd_max, fd_num;

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

	//register fd_set var
	FD_ZERO(&reads_init);
	FD_SET(server_sock, &reads_init);//monitor socket: server_sock
	FD_SET(0, &reads_init);// stdin also works
	fd_max = server_sock;
	//
	timeout_init.tv_sec = 5;
	timeout_init.tv_usec= 0;

	while(1){
		//调用select之后，除发生变化的文件描述符对应的bit，其他所有位置0，所以需用保存初值，通过复制使用
		reads = reads_init;
		//调用select之后，timeval成员值被置为超时前剩余的时间，因此使用时也需要每次用初值重新初始化
		timeout = timeout_init;
		fd_num = select(fd_max+1, &reads, NULL, NULL, &timeout);
		if(fd_num < 0){
			fputs("Error select()!", stderr);
			break;
		}else if(fd_num == 0){
			puts("Time-out!");
			continue;
		}
		for(i=0; i<=fd_max; i++){
			if(FD_ISSET(i, &reads)){
				if(i == server_sock){//connection request!
					//接受连接请求
					client_addr_size = sizeof(client_addr);
					client_sock = accept( server_sock, (struct sockaddr*)&client_addr, &client_addr_size );
					//accept函数自动创建数据I/0 socket
					if(-1 == client_sock){
						error_handling("accept() error");
						//健壮性不佳，程序崩溃退出
					} else{
						//注册与客户端连接的套接字文件描述符
						FD_SET(client_sock, &reads_init);
						if(fd_max < client_sock) fd_max = client_sock;
						printf("Connected client : %d\n", client_sock);
					}
				}else{//read message!
					str_len = read(i, buff, BUFF_SIZE);
					if(str_len){//echo to client
						buff[str_len] = 0;
						printf("Message from client %d: %s", i, buff);
						write(i, buff, str_len);
					}else{ //close connection
						FD_CLR(i, &reads_init);
						close(i);
						printf("Disconnected client %d!\n", i);
					}
				}//end of i==|!=server_sock
			}//end of if(FD_ISSET)
		}//end of while

	}//end of for

	//断开连接，关闭套接字
	close(server_sock);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}
