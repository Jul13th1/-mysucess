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

    int fd_first = open(FIFO,O_WRONLY);	//以只写的形式打开管道 1

    if(fd_first < 0)		//如果管道1小于零创建失败
    {
        perror("open:");
        return -1;
    }

    int fd_second = open(FIFO2,O_RDONLY); //以只写的形式打开管道 2

    if(fd_second < 0)		//如果管道2 小于零创建失败
    {
        perror("open:");
        return -1;
    }
    
    while(1)
    {
        char temp[100] = {0};
        printf("please input message to 2 :");	//对进程2 发送消息 
        scanf("%s",temp);
        while(getchar() != '\n');
        write(fd_first,temp,strlen(temp));	//对管道1 进行写操作

        memset(temp,0,100);
        read(fd_second,temp,sizeof(temp));	//对管道2 进行读操作
        printf("read from 2: %s\n",temp);	//打印出进程2 发送的消息
    }

    close(fd_first);
    close(fd_second);    //关闭两个管道
    return 0;
}