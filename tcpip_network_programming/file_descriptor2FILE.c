/*************************************************************************
	> File Name: file_descriptor2FILE.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月30日 星期一 14时08分11秒
  > Description:fdopen(),文件描述符转换为FILE结构体指针
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

int main(int argc, char *argv[])
{
	int fd;
	FILE *fp;

	if(2 != argc){
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_CREAT|O_RDWR|O_TRUNC);
	if(-1 ==fd){
		fputs("Error open()", stderr);
		exit(EXIT_FAILURE);
	}
	//文件描述符转换为FILE指针
	fp = fdopen(fd, "w");
	fputs("Linux TCP/IP network programming.\n", fp);
	//int fileno(FILE *stream); 取得FILE对应的文件描述符
	printf("File descriptor is %d, from FILE : %d\n", fd, fileno(fp));

	close(fp);
	return 0;
}
