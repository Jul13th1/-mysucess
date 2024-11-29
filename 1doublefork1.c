#include <stdio.h>
#include <stdalign.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main()
{
    int fd_first[2];
    int fd_second[2];	//创建两个管道

    if(pipe(fd_first) == -1)	//如果管道1创建失败，打印错误信息
    {
        perror("pipe:");
        return -1;
    }

    if(pipe(fd_second) == -1)	//如果管道2创建失败，打印错误信息
    {
        perror("pipe:");
        return -1;
    }

    pid_t pid = fork();		//创建进程

    if(pid < 0)			//如果小于零，创建失败，打印错误信息
    {
        perror("fork:");
        return -1;
    }

    if(pid == 0)    //child	//子进程
    {
        while(1)
        {
            char Buff[200] = {0};
            read(fd_first[0],Buff,sizeof(Buff));	//读 父进程发送的信息
            printf("收到来自父进程发送的消息 : %s\n",Buff); //收到信息提示

            memset(Buff,0,200);
		////每种类型的变量都有各自的初始化方法，memset() 函数可以说是初始化内存的“万能函数”，通常为新申请的内存进行初始化工作。它是直接操作内存空间，mem即“内存”（memory）的意思

            printf("向父进程发送消息:");
            scanf("%[^\n]",Buff);			//第二步：向父进程发送消息
            while(getchar() != '\n');
            write(fd_second[1],Buff,strlen(Buff));	//对管道信息进行写操作
        }
    }
    else            //parent	//父进程
    {
        while(1)
        {
            char buff[200] = {0};
            printf("向子进程发送消息: "); //第一步：向子进程发送消息
            scanf("%[^\n]",buff);
            while(getchar() != '\n');
            write(fd_first[1],buff,strlen(buff));	//对管道进行写操作

            memset(buff,0,200);
            
            read(fd_second[0],buff,sizeof(buff));	//读 父进程发送的信息
            printf("收到来自子进程的消息 : %s\n",buff); //收到信息提示

        }
        
        //printf("child process die!\n");
        //wait(NULL);
    }
    return 0;
}

			//运行结果
			//please input message to child: you are 1
			//read from parent process : you are 1
			//please input message to parent:you are 2
			//read from child process : you are 2
			//please input message to child: 