/*************************************************************************
	> File Name: readv_writev.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月29日 星期日 09时04分26秒
  > Description:
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<sys/uio.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	struct iovec vec[3];
	char buff1[BUFF_SIZE];
	char buff2[BUFF_SIZE];
	char buff3[BUFF_SIZE];
	int str_len, i;

	memset(buff1, 0, BUFF_SIZE);
	memset(buff2, 0, BUFF_SIZE);
	memset(buff3, 0, BUFF_SIZE);
	
	vec[0].iov_base = buff1;
	vec[0].iov_len = 2;
	vec[1].iov_base = buff2;
	vec[1].iov_len = 4;
	vec[2].iov_base = buff3;
	vec[2].iov_len = BUFF_SIZE - 1;

	puts("Input a message:");
	str_len = readv(0, vec, 3);
	printf("Read %d bytes : \n", str_len);
	printf("%s\n", buff1);
	printf("%s\n", buff2);
	printf("%s\n", buff3);
	printf("\n");
//	fputc('\n', stdout);

	str_len = writev(1, vec+1, 2);
	printf("Write %d bytes\n", str_len);

	return 0;
}
