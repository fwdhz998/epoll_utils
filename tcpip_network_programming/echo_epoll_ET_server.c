/*************************************************************************
	> File Name: echo_epoll_ET_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年04月03日 星期五 14时21分01秒
  > Description:
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<errno.h>
#include<fcntl.h>

void error_handling(char *message);
void set_nonblocking_mod(int fd);

#define BUFF_SIZE 4 //减小为
#define EPOLL_SIZE 30

int main(int argc, char *argv[])
{
	int sock_server;
	int sock_client;

	struct sockaddr_in addr_server;
	struct sockaddr_in addr_client;
	socklen_t size_addr_client;

	char buff[BUFF_SIZE];
	int str_len, i, errno;

	int epfd, count_event;
	struct epoll_event *ep_events;
	struct epoll_event event;
	

	if(argc!=2){ //命令行中启动服务程序仅限一个参数：端口号
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
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

	//epoll
	epfd = epoll_create(EPOLL_SIZE);
	//epfd = epoll_create(0); //epoll_wait() Error
	ep_events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	set_nonblocking_mod(sock_server); // Non-blocking mode socket
	event.events = EPOLLIN;//监视需用读取数据事件
	event.data.fd=sock_server;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sock_server, &event);
	//
	while(1){
		count_event = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
		if(count_event == -1){
			puts("epoll_wait() Error");
			break;
		}
		//检查epoll_wait()调用次数
		puts("Return epoll_wait()");

		for(i=0; i<count_event; i++){
			if(ep_events[i].data.fd == sock_server){
				//接受连接请求
				size_addr_client = sizeof(addr_client);
				sock_client = accept( sock_server, (struct sockaddr*)&addr_client, &size_addr_client);
				set_nonblocking_mod(sock_client);
				event.events = EPOLLIN|EPOLLET;//Non blocking
				event.data.fd = sock_client;
				epoll_ctl(epfd, EPOLL_CTL_ADD, sock_client, &event);
				printf("Connected client : %d\n", sock_client);
			}else{
				while(1){
					str_len = read(ep_events[i].data.fd, buff, BUFF_SIZE);
					if(str_len > 0){//echo to client
						buff[str_len] = 0;
						printf("Message from client %d: %s\n", i, buff);
						write(ep_events[i].data.fd, buff, str_len);//echo!
					}else if(str_len == 0){ //close connection
						epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
						close(ep_events[i].data.fd);
						printf("Disconnected client %d!\n", ep_events[i].data.fd);
						break;
					}else{
						if(errno == EAGAIN) break; //The buffer is empty.
					}
				}
			}//end of if()
		}//end of while
	}//end of for

	//断开连接，关闭套接字
	close(sock_server);
	close(epfd);//

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}
void set_nonblocking_mod(int fd)
{
	int flag;
	flag = fcntl(fd, F_GETFL, 0);
	fcntl(fd, flag|O_NONBLOCK);
}
