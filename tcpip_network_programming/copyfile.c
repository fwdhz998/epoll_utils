/*************************************************************************
	> File Name: copyfile.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月11日 星期三 14时26分07秒
  > Description:使用文件I/O实现文件复制功能
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

#define BUFSIZE 300

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int fd_read, fd_write;
	char buf[BUFSIZE];
	int bytes;

	if(3 != argc){
		printf("Usage : %s oldfile newfile", argv[0]);
		exit(1);
	}

	fd_read = open(argv[1], O_RDONLY);
	if(-1 == fd_read){
		error_handling("open() file for reading error!");
	}
	fd_write = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC);
	if(-1 == fd_write){
		error_handling("open() file for writing error!");
	}

	while((bytes = read(fd_read, buf, sizeof(buf)-1)) > 0){
		if( -1 == write(fd_write, buf, bytes) ){
			error_handling("write() error!");
		}
	}

	close(fd_read);
	close(fd_write);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
