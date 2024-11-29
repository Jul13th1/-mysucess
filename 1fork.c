#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>


//单向无名管道通信
int main()
{
    int fd[2];
    int num = 0;
    //判断管道是否创建成功
    if(pipe(fd) == -1)
    {
        perror("pipe:");
        return -1;
    }

    //创建子进程
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("pid:");
        return -1;
    }

    printf("pid- %d\n",pid);
    if(pid == 0)
    {
        printf("子进程：我在运行，PID = %d\n", getpid());
        char buffer[24] = "helloworld";
        num++;
        printf("子进程的num= %d\n",num);
        printf("子进程向父进程发送消息%s\n",buffer);
        write(fd[1],buffer,sizeof(buffer));
    }
    else
    {
        printf("父进程：我在运行，PID = %d\n", getpid());
        num++;
        printf("父进程的num= %d\n",num);
        char recvbuffer[20] = {0};
        read(fd[0],recvbuffer,sizeof(recvbuffer));
        printf("收到来自子进程的消息：%s\n",recvbuffer);
    }

    return 0;
}