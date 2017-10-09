/*************************************************************************
	> File Name: echo_stdio_client.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月30日 星期一 14时35分50秒
  > Description:基于 standard I/O 回声client
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *message);

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	int server_sock;
	FILE *fp_read, *fp_write;
	struct sockaddr_in server_addr;

	char message[BUF_SIZE];
	int str_len, recv_len, recv_cnt;

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
	//file descriptor 2 FILE	
	fp_read = fdopen(server_sock, "r");
	fp_write = fdopen(server_sock, "w");

	while(1){
		printf("Input message(Q to quit) : ");
		fgets(message, BUF_SIZE, stdin);

		if( 0 == strcmp(message, "q\n") || 0 == strcmp(message, "Q\n") ){
			break;
		}

		/*
		str_len = write(server_sock, message, strlen(message));
		recv_len = 0;
		while(recv_len < str_len){//< 降低因異常情況陷入無限循環
			recv_cnt = read(server_sock, &message[recv_len], BUF_SIZE-1-recv_len);
			if(-1 == recv_cnt){
				error_handling("read() error!");
			}
			recv_len += recv_cnt;
		}
		message[recv_len] = 0;
		*/
		//使用标准I/O之后可以按字符串单位进行数据交换
		fputs(message, fp_write);
		fflush(fp_write);
		fgets(message, BUF_SIZE, fp_read);
		printf("Message from server : %s \n", message);
	}

	fclose(fp_read);
	fclose(fp_write);
//	close(server_sock);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
