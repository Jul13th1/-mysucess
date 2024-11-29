#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

int main()
{
    pid_t pid = vfork();
    if(pid < 0)
    {
        perror("创建失败:");
        return -1;
    }

    if(pid == 0)
    {
        printf("我是子进程！\n");
        sleep(3);
    }
    else
    {
        printf("我是父进程\n");
    }
}