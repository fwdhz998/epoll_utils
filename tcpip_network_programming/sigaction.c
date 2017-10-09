/*************************************************************************
	> File Name: sigaction.c
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

int main(int argc, char *argv[])
{
	int i;
	struct sigaction sig_act;

	sig_act.sa_handler = timeout;
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;

	sigaction(SIGALRM, &sig_act, 0);
	alarm(2);

	for(i=0; i<3; i++){
		puts("waiting....");
		sleep(600);
	}
	return 0;
}
