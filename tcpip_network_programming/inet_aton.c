/*************************************************************************
	> File Name: inet_aton.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月12日 星期四 09时08分08秒
  > Description: 
  使用 inet_aton 函数将字符串形式IP地址转换为32位网络字节序整数，不过利用了in_addr结构图，更常用。其作用与inet_addr函数相同。
 ************************************************************************/

#include<stdio.h>
#include<arpa/inet.h>

int main(int argc, char *argv[])
{
	char *addr = "127.0.0.1";
	struct sockaddr_in addr_inet;

	if(0 == inet_aton(addr, &(addr_inet.sin_addr))){
		printf("Error\n");
	} else {
		printf("Network ordered integer addr : %#x \n", addr_inet.sin_addr.s_addr);
	}

	return 0;
}
