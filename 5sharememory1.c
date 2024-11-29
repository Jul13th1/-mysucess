#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>

int main()
{
    //创建一个键值
    key_t key = ftok("./",12);
    if(key < 0)
    {
        perror("key:");
        return -1;
    }

    //创建一个共享内存 并返回一个共享内存标识符
    //共享内存储存的字节数为1024 拥有读写权限
    int shmID = shmget(key,1024,IPC_CREAT|0666);
    printf("shmid: %d\n",shmID);

    //允许本进程访问创建的关系内存 放在一个合适的地址
    void *text = shmat(shmID,NULL,0);

    printf("read from shm: %s\n",(char*)text);

    //将共享内存从当前进程分离
    if(shmdt(text) != 0)
    {
        perror("shmdt:");
        return -1;
    }

    //删除共享内存
    shmctl(shmID,IPC_RMID,NULL);

    return 0;
}