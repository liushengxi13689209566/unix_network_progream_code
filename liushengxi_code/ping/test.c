/*************************************************************************
	> File Name: test.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年10月09日 星期二 11时55分54秒
 ************************************************************************/

#include<stdio.h>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char **argv)
{
   int ch;
   opterr = 0;
   while((ch = getopt(argc, argv, ":abcde")) != -1)
   switch(ch)
   {
      case 'a':
         printf("option %c'\n", ch);  break;
      case 'b':
         printf("option %c \n",ch);  break;
      default:
         printf("other option :%c\n", ch);
   }
   printf("optopt +%c\n", optopt);
}

