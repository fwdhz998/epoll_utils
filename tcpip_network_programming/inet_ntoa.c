/*************************************************************************
	> File Name: inet_aton.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月12日 星期四 09时22分48秒
  > Description: 
  使用 inet_ntoa 函数将32位网络字节序整数IP地址转换为字符串形式IP地址。其作用与 inet_aton 函数相反。
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>

int main(int argc, char *argv[])
{
	char str_addr[20];
	char *str_ptr;
	struct sockaddr_in sockaddr;

	sockaddr.sin_addr.s_addr = htonl(0x1020304);
	str_ptr = inet_ntoa(sockaddr.sin_addr);
	strcpy(str_addr, str_ptr);
	printf("Dotted-Decimal notation IP addr : %s \n", str_addr);

	return 0;
}
