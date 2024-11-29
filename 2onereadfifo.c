#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO "oneFIFO"
int main()
{
    //创建管道并赋予读写权限
    mkfifo(FIFO,S_IRUSR | S_IWUSR);
    //以只写的方式打开文件
    int fd = open(FIFO,O_WRONLY);

    //判断是否打开成功
    if(fd < 0)
    {
        perror("open:");
        return -1;
    }

    while(1)
    {
        char buffer[20] = {0};
        printf("向文件输入：");
        scanf("%[^\n]",buffer);
        // memset(buffer,0,100);
        // fgets(buffer, sizeof(buffer), stdin); // 使用 fgets 处理输入
        //向管道文件写入字符串
        while(getchar() != '\n');
        write(fd,buffer,strlen(buffer));
    }
    
    close(fd);

    return 0;
}