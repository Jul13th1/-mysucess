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
    // //指定消息类型
    // msg.type = 2;
    // //msg.tep的意思是 这条消息队列  要返回的消息标志位2
    // //将要发送的消息复制到结构体里
    // strcpy(msg.connect,"来自标志2的消息");
    // //将消息放入消息队列中
    // if(msgsnd(msgid,&msg,sizeof(MSG),0) == -1)
    // {
    //     perror("msgsnd:");
    //     return -1;
    // }

    //优先级发送
     while (1) 
     {
        printf("请输入消息类型（1-高，2-中，3-低）：");
        scanf("%ld", &msg.type);
        printf("请输入消息内容：");
        scanf("%s", msg.connect);  // 输入消息内容

        if (msgsnd(msgid, &msg, sizeof(MSG), 0) == -1) 
        {
            perror("msgsnd:");
            return -1;
        }

        printf("消息发送成功，类型: %ld, 内容: %s\n", msg.type, msg.connect);
    }

    return 0;

}