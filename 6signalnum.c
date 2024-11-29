#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

void PV(int semID,int op)
{
    struct sembuf buf;	//sembuf 用于' semop'参数以描述操作的结构
    buf.sem_num = 0; //信号量的编号，如果你的工作不需要使用一组信号量，这个值一般就取为0
    buf.sem_flg = SEM_UNDO; //通常被设置为SEM_UNDO.她将使操作系统跟踪当前进程对该信号量的修改情                     				//况
    buf.sem_op = op; //是信号量一次PV操作时加减的数值，一般只会用到两个值，一个是“－1”，也就是P操作，等待信号量变得可用；另一个是“＋1”，也就是我们的V操作，发出信号量已经变得可用

    semop(semID,&buf,1);   //对信号量进行操作，一个结构体
}

int main()
{
    key_t key = ftok("./",20);	//创建一个IPC键值
    
    int semID = semget(key,1,IPC_CREAT | 0666);	//创建一个新的信号量 数量为1 权限为读写
    if(semID < 0)	//如果信号量小于0 
    {
        perror("semget:");	//打印失败的信息
        return -1;		//返回值为-1
    }
    semctl(semID,0,SETVAL,1);	//设置第一个信号量的初始值为1  0是信号量的第一个下标
    
    pid_t pid = fork();		//创建子进程
    
    if(pid < 0)
    {
        perror("fork:");
        return -1;
    }
    
    if(pid == 0)		//子进程
    {
        while(1)
        {
            PV(semID,-1);	//申请信号量
            printf("i am child process!\n");
            sleep(2);
            printf("child am awak!\n");
            PV(semID,1);	//释放信号量

        }
    }
    else
    {
        while(1)
        {
            PV(semID,-1);	//申请信号量
            printf("i am parent process!\n");
            sleep(2);
            printf("parent is awak!\n");
            PV(semID,1);	//释放信号量
        }
    }

    semctl(semID,0,IPC_RMID);	//删除信号量

    
    return 0;
}
