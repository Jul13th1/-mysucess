#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork:");
        return -1;
    }

    if(pid == 0)
    {
        while(1)
        {
            printf("我是子进程！\n");
            sleep(1);
        }
    }
    else
    {
        sleep(5);
        printf("向子进程发送信号\n");
        kill(pid,SIGINT);
        wait(NULL);
    }   

    return 0;
}