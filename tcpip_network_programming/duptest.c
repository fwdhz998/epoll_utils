/*************************************************************************
	> File Name: duptest.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月31日 星期二 09时11分31秒
  > Description: 复制文件描述符，实质时创建一个指向相同文件或套接字的新文件描述符
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
	int fd1, fd2;
	char msg1[] = "Hi, \n";
	char msg2[] = "I am a genuis.\n";

	fd1 = dup(1);
	fd2 = dup2(1, 9);

	printf("New file descriptor: fd1=%d, fd2=%d\n", fd1, fd2);
	write(fd1, msg1, sizeof(msg1));
	write(fd2, msg2, sizeof(msg2));

	close(fd1);
	close(fd2);

	write(1, msg1, sizeof(msg1));
	close(1);//close standard output
	write(1, msg2, sizeof(msg2));

	return 0;
}
