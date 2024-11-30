#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9999
#define BUFFER_LENGTH 1024

int main() 
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_LENGTH] = "Hello, Server!";

    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("socket");
        return -1;
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) 
    {
        perror("inet_pton");
        return -1;
    }

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("connect");
        return -1;
    }

    // 发送数据
    send(sockfd, buffer, strlen(buffer), 0);

    // 接收数据
    int len = recv(sockfd, buffer, BUFFER_LENGTH, 0);
    if (len > 0) 
    {
        buffer[len] = '\0';  // 添加字符串结束符
        printf("Received from server: %s\n", buffer);
    }

    // 关闭连接
    close(sockfd);

    return 0;
}