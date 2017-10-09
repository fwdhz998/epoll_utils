/*************************************************************************
	> File Name: signal.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月25日 星期三 14时50分00秒
  > Description:
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<signal.h>

void timeout(int sig)
{
	if(sig == SIGALRM)
		puts("Time out!");
	alarm(2);
}
void keycontrol(int sig)
{
	if(sig == SIGINT)
		puts("CTRL+C pressed.");
}

int main(int argc, char *argv[])
{
	int i;
	signal(SIGALRM, timeout);
	signal(SIGINT, keycontrol);
	alarm(2);

	for(i=0; i<3; i++){
		puts("waiting....");
		sleep(600);
	}
	return 0;
}
