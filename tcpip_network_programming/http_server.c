/*************************************************************************
	> File Name: http_server.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年04月03日 星期五 21时17分48秒
  > Description: a simple 基于Linux的多线程Web服务器
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>

#define BUFF_SIZE 1024
#define BUFF_SMALL 100

void *request_handler(void *arg);
void send_data(FILE *fp, char *ct, char *file_name);
void *content_type(char *file);
void send_error(FILE *fp);
void error_handler(char *message);

int main(int argc, char *argv[])
{
	int sock_server, sock_client;
	struct sockaddr_in addr_server, addr_client;
	int size_addr_client;
	char buff[BUFF_SIZE];
	pthread_t pthread_id;

	if(argc != 2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	sock_server = socket(PF_INET, SOCK_STREAM, 0);
	if(-1 == sock_server){
		error_handler("Error socket()");
	}

	memset(&addr_server, 0, sizeof(addr_server));
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_server.sin_port = htons(atoi(argv[1]));

	if(-1 == bind(sock_server, (struct sockaddr*)&addr_server, sizeof(addr_server))){
		error_handler("Error bind()");
	}

	if(-1 == listen(sock_server, 5)){
		error_handler("Error listen()");
	}

	while(1){
		size_addr_client = sizeof(addr_client);
		sock_client = accept(sock_server, (struct sockaddr*)&addr_client, &size_addr_client);
		printf("Connection Request: %s:%d\n", inet_ntoa(addr_client.sin_addr), ntohs(addr_client.sin_port));
		pthread_create(&pthread_id, NULL, request_handler, &sock_client);
		pthread_detach(pthread_id);
	}

	close(sock_server);
	return 0;
}
void error_handler(char *msg)
{
	fputs(msg, stderr);
	fputs("\n", stderr);
	exit(1);
}

void *request_handler(void *arg)
{
	int sock_client = *((int *)arg);
	char req_line[BUFF_SMALL];
	FILE *read_fp;
	FILE *write_fp;

	char method[10];
	char ct[15];
	char file_name[30];

	read_fp = fdopen(sock_client, "r");
	write_fp = fdopen(dup(sock_client), "w");

	fgets(req_line, BUFF_SMALL, read_fp);
	if(strstr(req_line, "HTTP/")==NULL){
		fputs("Error 1\n", stderr);
		send_error(write_fp);
		fclose(read_fp);
		fclose(write_fp);
		return;
	}
	strcpy(method, strtok(req_line, " /"));
	strcpy(file_name, strtok(NULL, " /"));
	strcpy(ct, content_type(file_name));
	if(strcmp(method, "GET") != 0){
		fputs("Error 2\n", stderr);
		send_error(write_fp);
		fclose(read_fp);
		fclose(write_fp);
		return;
	}
	fclose(read_fp);
	send_data(write_fp, ct, file_name);
}

void send_data(FILE *fp, char *ct, char *file_name)
{
	char protocol[] = "HTTP/1.0 200 OK\r\n";
	char server[] = "Server: Linux web Server \r\n";
	char cnt_len[] = "Content-length:2048\r\n";
	char cnt_type[BUFF_SMALL];
	char buff[BUFF_SIZE];
	FILE *send_fp;

	sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);
	send_fp = fopen(file_name, "r");
	if(send_fp == NULL){
		fputs("Error 3\n", stderr);
		send_error(fp);
		return;
	}
	//Send Head Info
	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);
	//Send requested data
	while(fgets(buff, BUFF_SIZE, send_fp) != NULL){
		fputs(buff, fp);
		fflush(fp);
	}
	fflush(fp);
	fclose(fp);
}
void *content_type(char *file)
{
	char extension[BUFF_SMALL];
	char file_name[BUFF_SMALL];
	strcpy(file_name, file);
	strtok(file_name, ".");
	strcpy(extension, strtok(NULL, "."));

	if(!strcmp(extension, "html") || !strcmp(extension, "htm")){
		return "text/html";
	}else{
		return "text/plain";
	}
}
void send_error(FILE *fp)
{
	char protocol[] = "HTTP/1.0 4xx Bad Request\r\n";
	char server[] = "Server: Linux Web Server \r\n";
	char cnt_len[] = "Content-length:2048\r\n";
	char cnt_type[] = "Content-type:text/html\r\n\r\n";
	char content[] = "<html><head><title>NetWrok</title></head>"
		"<body><font size=+5><br>Error! Look up the request method and filename "
		"</font></body></html>";
	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);
	fputs(content, fp);
	fflush(fp);
}
