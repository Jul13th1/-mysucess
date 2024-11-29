#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>

//创建消息结构体
struct Message
{
    long type;  //消息类型
    char connect[100];
};

typedef struct Message MSG;

int main()
{
    //创建一个键值
    //"./" 是用于生成唯一键的路径，通常指向程序运行时的当前工作目录
    key_t key = ftok("./",10);

    if(key < 0)			//如果值小于零 创建失败
    {
        perror("ftok:");	//打印失败的原因
    }

    //创建一个消息队列 使其拥有读写权限
    int msgid = msgget(key,IPC_CREAT | 0777);
    if(msgid < 0)
    {
        perror("msgget:");
        return -1;
    }

    MSG msg;
    //从消息队列中读取一条 刚刚在发送端设置的 2号标志消息

    // msgrcv(msgid,&msg,sizeof(MSG),2,0);
    // printf("收到来自消息队列发送的消息：%s\n",msg.connect);

    // //删除消息队列
    // msgctl(msgid,IPC_RMID,NULL);

     while (1) 
     {
        // 按优先级接收消息
        for (long priority = 1; priority <= 3; priority++) 
        {
            if (msgrcv(msgid, &msg, sizeof(MSG) - sizeof(long), priority, IPC_NOWAIT) != -1) 
            {
                printf("接收到消息，类型: %ld, 内容: %s\n", msg.type, msg.connect);
            }
        }
        usleep(100000);  // 等待一段时间再检查
    }

    return 0;

}