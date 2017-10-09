/*************************************************************************
	> File Name: syscpy.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月30日 星期一 10时29分51秒
  > Description: 使用 system function 实现文件复制功能
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

#define BUFF_SIZE 3 //使用最短数组长度

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int fd_read, fd_write;
	char buff[BUFF_SIZE];
	int bytes;

	if(3 != argc){
		printf("Usage : %s <oldfile> <newfile>\n", argv[0]);
		exit(1);
	}

	fd_read = open(argv[1], O_RDONLY);
	if(-1 == fd_read){
		error_handling("open() file for reading error!");
	}
	fd_write = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC);//拷贝之后的txt文件存在read write属性与原文件不匹配的现象
	if(-1 == fd_write){
		error_handling("open() file for writing error!");
	}

	while((bytes = read(fd_read, buff, sizeof(buff))) > 0){
		if( -1 == write(fd_write, buff, bytes) ){
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
