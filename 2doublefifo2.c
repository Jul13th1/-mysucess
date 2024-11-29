#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define FIFO "MyFIFO"
#define FIFO2 "MyFIFO2"

int main()
{
    mkfifo(FIFO,S_IRUSR | S_IWUSR);       //构建有名管道 拥有者的读写权限
    mkfifo(FIFO2,S_IRUSR | S_IWUSR);

    //open(FIFO,O_RDONLY);         

    int fd_first = open(FIFO,O_RDONLY);   //以只读的形式打开管道1

    if(fd_first < 0)			//如果管道1 小于零 创建失败 打印错误信息
    {
        perror("open:");
        return -1;
    }

    int fd_second = open(FIFO2,O_WRONLY);   //以只写的形式打开管道2

    if(fd_second < 0)			//如果管道2 小于零 创建失败 打印错误信息
    {
        perror("open:");
        return -1;
    }

    while(1)
    {
        char temp[100] = {0};
        read(fd_first,temp,sizeof(temp));	//收到进程1 发送的消息 进行读操作
        printf("read from 1: %s\n",temp);	//打印收到的消息

        printf("please input message to 1 :");	//对进程1 发送消息
        memset(temp,0,100);
        scanf("%s",temp);
        while(getchar() != '\n');
        write(fd_second,temp,strlen(temp));	//对管道2 进行写操作
    }
    close(fd_first);
    close(fd_second);    //关闭管道
    return 0;
}