// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/types.h>
// #include <unistd.h>

// int main()
// {
//     int first_fd[2];
//     int second_fd[2];

//     if(pipe(first_fd) == -1 && pipe(second_fd) == -1)
//     {
//         perror("pipe:");
//         return -1;
//     }

//     pid_t pid = vfork();
//     if(pid < 0)
//     {
//         perror("pid:");
//         return -1;
//     }
    
//     //使用vfork进行父子进程通信 使用两个管道
//     if(pid == 0)
//     {
//         while(1)
//         {
//             char buffer[20] = {0};
//             printf("向父进程发送消息：");
//             scanf("%[^\n]",buffer);
//             write(first_fd[1],buffer,sizeof(buffer));

//             //将buffer数组里面的元素初始化为0
//             memset(buffer,0,100);
//             read(second_fd[0],buffer,sizeof(buffer));
//             printf("收到来自父进程的消息：%s\n",buffer);
//         }
//     }
//     else
//     {
//         while(1)
//         {
//             char buffer[20] = {0};
//             read(first_fd[0],buffer,sizeof(buffer));
//             printf("收到来自子进程的消息：%s\n",buffer);

//             memset(buffer,0,100);
//             printf("向子进程发送消息：");
//             scanf("%[^\n]",buffer);
//             write(second_fd[1],buffer,sizeof(buffer));
//         }

//     }

//     return 0;

// }