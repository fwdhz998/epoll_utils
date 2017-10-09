/*************************************************************************
	> File Name: stdcpy.c
	> Author: xjhznick
	> Mail: xjhznick@gmail.com 
	> Created Time: 2015年03月30日 星期一 10时33分37秒
  > Description:使用带缓冲的 standard I/O 进行 Text 文件复制操作
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h> // exit()

#define BUFF_SIZE 3 //使用最短数组长度

void error_handling(char *message);

int main(int argc, char *argv[])
{
	FILE *fp_read, *fp_write;
	char buff[BUFF_SIZE];

	if(3 != argc){
		printf("Usage : %s <oldfile> <newfile>\n", argv[0]);
		exit(1);
	}

	fp_read = fopen(argv[1], "r");
	if(NULL == fp_read){
		error_handling("fopen() file for reading error!");
	}
	fp_write = fopen(argv[2], "w");
	if(NULL == fp_write){
		error_handling("fopen() file for writing error!");
	}

	while(fgets(buff, BUFF_SIZE, fp_read) != NULL){
		fputs(buff, fp_write);
	}
	/*
	while((ch=getc(fp))!=EOF)
		putc(ch, stdout);//putchar(ch);
		//int fputc(int ch, FILE *fp); putc() === fputc
	*/

	fclose(fp_read);
	fclose(fp_write);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
