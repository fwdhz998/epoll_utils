/*************************************************************************
	> File Name: seperator_io_client.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月31日 星期二 09时26分28秒
  > Description:
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *message);

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	int sock_server;
	FILE *fp_read, *fp_write;
	struct sockaddr_in addr_server;

	char message[BUFF_SIZE];

	if(3 != argc){
		printf("Usage : %s <IP> <Port> \n", argv[0]);
		exit(1);
	}

	sock_server = socket(PF_INET, SOCK_STREAM, 0);
	if(-1 == sock_server){
		error_handling("socket() error!");
	}

	memset(&addr_server, 0, sizeof(addr_server));
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = inet_addr(argv[1]);
	addr_server.sin_port = htons(atoi(argv[2]));

	if( -1 == connect(sock_server, (struct sockaddr*)&addr_server,
					  sizeof(addr_server)) ){
		error_handling("connect() error!");
	}else{
		puts("Connected......");
	}
	//file descriptor 2 FILE	
	fp_read = fdopen(sock_server, "r");
	fp_write = fdopen(sock_server, "w");

	while(1){
		if(fgets(message, BUFF_SIZE, fp_read) == NULL) break;
		
		fputs(message, stdout);
		fflush(stdout);
	}

	fputs("From Client: Thank you!\n", fp_write);
	fflush(fp_write);

	fclose(fp_read);
	fclose(fp_write);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
